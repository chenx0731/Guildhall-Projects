#define UNUSED(x) (void)(x);
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


const Rgba8 DevConsole::ERROR = Rgba8::RED;
const Rgba8 DevConsole::WARNING = Rgba8(255, 255, 0);
const Rgba8 DevConsole::INFO_MAJOR = Rgba8(255, 0, 255);
const Rgba8 DevConsole::INFO_MINOR = Rgba8(255, 0, 255, 127);
const Rgba8 DevConsole::REMOTE_ECHO = Rgba8(100, 0, 255);
const Rgba8 DevConsole::COMMAND_ECHO = Rgba8::GREY;
const Rgba8 DevConsole::INPUT_TEXT = Rgba8::WHITE;
const Rgba8 DevConsole::INPUT_CARET = Rgba8::WHITE;


BitmapFont* g_consoleFont = nullptr;

DevConsole::DevConsole(DevConsoleConfig const& config) : m_config(config)
{
	//g_consoleFont = 
	m_caretStopwatch = new Stopwatch(&Clock::GetSystemClock(), 0.5f);
}

DevConsole::~DevConsole()
{
}

void DevConsole::Startup()
{
	std::string fontFile = "Data/Fonts/" + m_config.m_fontName;
	g_consoleFont = m_config.m_renderer->CreateOrGetBitmapFont(fontFile.c_str());
	SubscribeEventCallbackFunction("KeyPressed", Event_KeyPressed);
	SubscribeEventCallbackFunction("CharInput", Event_CharInput);
	SubscribeEventCallbackFunction("Help", Command_Help);
	SubscribeEventCallbackFunction("Clear", Command_Clear);
	SubscribeEventCallbackFunction("FailToFire", FailToFireComment);
}

void DevConsole::Shutdown()
{
	delete g_consoleFont;
	delete m_caretStopwatch;
	m_caretStopwatch = nullptr;
	g_consoleFont = nullptr;
}

void DevConsole::BeginFrame()
{
	if (m_caretStopwatch->DecrementDurationIfElapsed()) {
		m_caretVisible = !m_caretVisible;
		m_caretStopwatch->Restart();
	}
}

void DevConsole::EndFrame()
{
}

void DevConsole::Execute(std::string const& consoleCommandText)
{
	
	Strings commandArgs = SplitStringWithQuotes(consoleCommandText, " ");
		//SplitStringOnDelimiter(consoleCommandText, " ");
	EventArgs eventArgs;
	if (commandArgs.size() > 1) {
		for (int argIndex = 1; argIndex < commandArgs.size(); argIndex++) {
			Strings nameAndValue = SplitStringWithQuotes(commandArgs[argIndex].c_str(), "=");
			if (nameAndValue.size() > 1) {
				eventArgs.SetValue(nameAndValue[0], nameAndValue[1]);
			}
		}
	}
	if (!CheckIfAEvent(commandArgs[0]) && commandArgs[0] != "Echo") {
		//AddLine(DevConsole::ERROR, consoleCommandText);
		AddLine(DevConsole::ERROR, "Unknow Command: " + consoleCommandText);
		m_caretPosition = 0;
		m_inputText.clear();
		return;
	}
	
	m_isFired = true;
	FireEvent(commandArgs[0], eventArgs);
	if (m_isFired) {
		if (commandArgs[0] == "Echo") {
			Strings argsWithQuotes = SplitStringWithQuotes(consoleCommandText, " ");
			if ((int)argsWithQuotes.size() > 1) {
				std::string message = argsWithQuotes[1];
				Strings echoArgs = SplitStringWithQuotes(message, "=");
				
				if ((int)echoArgs.size() > 1) {
					TrimString(echoArgs[1], "\"");
					AddLine(DevConsole::REMOTE_ECHO, echoArgs[1]);
				}
				//AddLine(DevConsole::REMOTE_ECHO, echoArgs[1]);
			}
		}
		else
		AddLine(DevConsole::COMMAND_ECHO, consoleCommandText);

		m_commandHistory.push_back(consoleCommandText);
		m_historyIndex = (int)m_commandHistory.size() - 1;
	}
	m_caretPosition = 0;
	m_inputText.clear();
	
}

void DevConsole::AddLine(Rgba8 const& color, std::string const& text)
{
	m_linesLock.lock();
	DevConsoleLine newLine;
	newLine.m_color = color;
	newLine.m_text = text;
	m_lines.push_back(newLine);
	m_linesLock.unlock();
}

void DevConsole::ExecuteXmlCommandScriptNode(XmlElement const& commandScriptXmlElement)
{
	//XmlElement* childElement = commandScriptXmlElement->FirstChildElement();
	if (&commandScriptXmlElement != nullptr)
	{
		std::string text = commandScriptXmlElement.Name();

		const XmlAttribute* attribute = commandScriptXmlElement.FirstAttribute();

		while (attribute != nullptr)
		{
			std::string attributeText = " ";
			attributeText += attribute->Name();
			attributeText += "=\"";
			attributeText += attribute->Value();
			attributeText += "\"";
			attribute = attribute->Next();
			text += attributeText;
		}
		Execute(text);

		//commandScriptXmlElement = commandScriptXmlElement.FirstChildElement();
	}

}


void DevConsole::ExecuteXmlCommandScriptFile(std::string const& commandScriptXmlFilePathName)
{
	XmlDocument commandDoc;
	bool isFileValid = IsFileExist(commandScriptXmlFilePathName);
	if (!isFileValid) {
		ERROR_AND_DIE(Stringf("Can't open file: %s", commandScriptXmlFilePathName));
	}
	commandDoc.LoadFile(commandScriptXmlFilePathName.c_str());
	XmlElement* commandDefElement = commandDoc.RootElement();
	commandDefElement = commandDefElement->FirstChildElement();
	while (commandDefElement)
	{
		ExecuteXmlCommandScriptNode(*commandDefElement);
		commandDefElement = commandDefElement->NextSiblingElement();
	}
	
	//std::string text = modelDefElement->
	//std::string = modelDefElement->ToText();
}

void DevConsole::Render(AABB2 const& bounds)
{
	if (!m_isOpen)
		return;
	//m_linesLock.lock();
	float cellHeight = bounds.GetDimensions().y / m_config.m_linesOnScreen;
	m_config.m_renderer->BeginCamera(*m_config.m_camera);
	m_config.m_renderer->SetModelConstants();
	std::vector<Vertex_PCU> squaredVerts;
	AddVertsForAABB2D(squaredVerts, bounds, Rgba8(0, 0, 0, 100));
	m_config.m_renderer->BindTexture(nullptr);
	m_config.m_renderer->DrawVertexArray((int)squaredVerts.size(), squaredVerts.data());
	std::vector<Vertex_PCU> textVerts;
	std::vector<Vertex_PCU> shadowVerts;
	Vec2 boundMins = bounds.m_mins;
	Vec2 boundMaxs = Vec2(bounds.m_maxs.x, bounds.m_mins.y + cellHeight);
	Vec2 offset = Vec2(2.f, 2.f);
	if (m_caretVisible)
		g_consoleFont->AddVertsForTextInBox2D(textVerts, AABB2(boundMins, boundMaxs), cellHeight, "|", DevConsole::INPUT_CARET, m_config.m_fontAspect, Vec2((cellHeight * m_config.m_fontAspect * float(m_caretPosition) - 5.f) / (boundMaxs.x - boundMins.x), 0.f));
	g_consoleFont->AddVertsForTextInBox2D(textVerts, AABB2(boundMins, boundMaxs), cellHeight, m_inputText, DevConsole::INPUT_TEXT, m_config.m_fontAspect, Vec2(0.f, 0.f));
	m_config.m_renderer->BindTexture(&g_consoleFont->GetTexture());
	for (int lineIndex = (int)m_lines.size() - 1; lineIndex >= 0; lineIndex--) {
		boundMins.y += cellHeight;
		boundMaxs.y += cellHeight;

		if (boundMaxs.y > bounds.m_maxs.y)
			break;
		g_consoleFont->AddVertsForTextInBox2D(textVerts, AABB2(boundMins, boundMaxs), cellHeight, m_lines[lineIndex].m_text, m_lines[lineIndex].m_color, m_config.m_fontAspect, Vec2(0.f, 0.0f));
		g_consoleFont->AddVertsForTextInBox2D(shadowVerts, AABB2(boundMins - offset, boundMaxs - offset), cellHeight, m_lines[lineIndex].m_text, Rgba8::BLACK, m_config.m_fontAspect, Vec2(0.f, 0.f));
	}
	m_config.m_renderer->DrawVertexArray((int)shadowVerts.size(), shadowVerts.data());
	m_config.m_renderer->DrawVertexArray((int)textVerts.size(), textVerts.data());
	m_config.m_renderer->BindTexture(nullptr);
	m_config.m_renderer->EndCamera(*m_config.m_camera);
	//m_linesLock.unlock();
}

void DevConsole::ToggleOpen()
{
	m_isOpen = !m_isOpen;
	if (m_isOpen) {
		m_caretStopwatch->Start();
	}
}

bool DevConsole::IsOpen()
{
	return m_isOpen;
}

bool DevConsole::Event_KeyPressed(EventArgs& args)
{
	unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", -1);
	if (keyCode == KEYCODE_TILDE) {
		g_theDevConsole->ToggleOpen();
	}
	if (!g_theDevConsole->m_isOpen) {
		return false;
	}
	if (keyCode == KEYCODE_DELETE) {
		std::string suffix = g_theDevConsole->m_inputText.substr(g_theDevConsole->m_caretPosition);
		if (suffix.size() > 0) {
			std::string temp = g_theDevConsole->m_inputText;
			g_theDevConsole->m_inputText = temp.substr(0, g_theDevConsole->m_caretPosition) + temp.substr(g_theDevConsole->m_caretPosition + 1);
		}
	}
	if (keyCode == KEYCODE_HOME) {
		g_theDevConsole->m_caretPosition = 0;
	}
	if (keyCode == KEYCODE_END) {
		g_theDevConsole->m_caretPosition = (int)g_theDevConsole->m_inputText.size();
	}
	if (keyCode == KEYCODE_RIGHTARROW) {
		if (g_theDevConsole->m_caretPosition < (int)g_theDevConsole->m_inputText.size())
			g_theDevConsole->m_caretPosition++;
	}
	if (keyCode == KEYCODE_LEFTARROW) {
		if (g_theDevConsole->m_caretPosition > 0)
			g_theDevConsole->m_caretPosition--;
	}
	if (keyCode == KEYCODE_UPARROW) {
		if (g_theDevConsole->m_historyIndex > 0) {
			g_theDevConsole->m_historyIndex--;
		}
		else if(g_theDevConsole->m_historyIndex == 0) {
			g_theDevConsole->m_historyIndex = (int)g_theDevConsole->m_commandHistory.size() - 1;
		}
		if (g_theDevConsole->m_commandHistory.size() > 0)
		g_theDevConsole->m_inputText = g_theDevConsole->m_commandHistory[g_theDevConsole->m_historyIndex];
			
	}
	if (keyCode == KEYCODE_DOWNARROW) {
		if (g_theDevConsole->m_historyIndex < g_theDevConsole->m_commandHistory.size() - 1) {
			g_theDevConsole->m_historyIndex++;
		}
		else if (g_theDevConsole->m_historyIndex >= g_theDevConsole->m_commandHistory.size() - 1) {
			g_theDevConsole->m_historyIndex = 0;
		}
		if (g_theDevConsole->m_commandHistory.size() > 0)
		g_theDevConsole->m_inputText = g_theDevConsole->m_commandHistory[g_theDevConsole->m_historyIndex];
	}
	if (keyCode == KEYCODE_BACKSPACE) {
		std::string prefix = g_theDevConsole->m_inputText.substr(0, g_theDevConsole->m_caretPosition);
		if (prefix.size() > 0) {
			std::string temp = g_theDevConsole->m_inputText;
			--g_theDevConsole->m_caretPosition;
			g_theDevConsole->m_inputText = temp.substr(0, g_theDevConsole->m_caretPosition) + temp.substr(g_theDevConsole->m_caretPosition + 1);
			//g_theDevConsole->m_caretPosition--;
		}
		return true;
	}
	if (keyCode == KEYCODE_ENTER) {
		if (g_theDevConsole->m_inputText.empty()) {
			g_theDevConsole->m_isOpen = false;
		}
		else g_theDevConsole->Execute(g_theDevConsole->m_inputText);
	}
	if (keyCode == KEYCODE_ESC) {
		if (g_theDevConsole->m_inputText.empty())
			g_theDevConsole->ToggleOpen();
		else {
			g_theDevConsole->m_inputText.clear();
			g_theDevConsole->m_caretPosition = 0;
		}
	}
	g_theDevConsole->m_caretVisible = true;
	return true;
}


bool DevConsole::Event_CharInput(EventArgs& args)
{
	if (g_theDevConsole == nullptr) {
		return false;
	}
	unsigned char charCode = (unsigned char)args.GetValue("CharCode", -1);
	if (g_theDevConsole->m_isOpen) {
		if (charCode > 126 || charCode < 32 || charCode == '`') {
			//|| charCode == KEYCODE_TILDE || charCode == '~') {
			return true;
		}
		std::string tempString = g_theDevConsole->m_inputText;
		if (g_theDevConsole->m_caretVisible)
			g_theDevConsole->m_inputText = tempString.substr(0, g_theDevConsole->m_caretPosition) + (char)charCode + tempString.substr((g_theDevConsole->m_caretPosition++));
		else 
			g_theDevConsole->m_inputText = tempString.substr(0, g_theDevConsole->m_caretPosition) + (char)charCode + tempString.substr(g_theDevConsole->m_caretPosition++);
		return true;
	}
	return false;
}

bool DevConsole::Command_Clear(EventArgs& args)
{
	UNUSED(args);
	if (g_theDevConsole == nullptr) {
		return false;
	}
	g_theDevConsole->m_linesLock.lock();
	g_theDevConsole->m_lines.clear();
	g_theDevConsole->m_linesLock.unlock();
	return true;
}

bool DevConsole::Command_Help(EventArgs& args)
{
	UNUSED(args);
	if (g_theDevConsole == nullptr) {
		return false;
	}
	g_theDevConsole->AddLine(DevConsole::INFO_MAJOR, "Register Commands");
	Strings events = GetAllEvents();
	for (int eventIndex = 0; eventIndex < events.size(); eventIndex++) {
		g_theDevConsole->AddLine(DevConsole::INFO_MINOR, events[eventIndex]);
	}
	return true;
}


bool DevConsole::FailToFireComment(EventArgs& args)
{
	UNUSED(args);
	if (g_theDevConsole == nullptr) {
		return false;
	}
	g_theDevConsole->m_isFired = false;
	return true;
}
