#pragma once
#include "Engine/Core/BufferWriter.hpp"
#include "Engine/Core/BufferParser.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

//------------------------------------------------------------------------------------------------
void AppendTestFileBufferData(BufferWriter& bufWrite, EndianMode endianMode);
void ParseTestFileBufferData(BufferParser& bufParse, EndianMode endianMode);

void AppendTestFileBufferData(BufferWriter& bufWrite, EndianMode endianMode)
{
	bufWrite.SetEndianMode( endianMode );
	bufWrite.AppendChar( 'T' );
	bufWrite.AppendChar( 'E' );
	bufWrite.AppendChar( 'S' );
	bufWrite.AppendChar( 'T' );
	bufWrite.AppendByte( 2 ); // Version 2
	bufWrite.AppendByte( (unsigned char) bufWrite.GetEndianMode() );
	bufWrite.AppendBool( false );
	bufWrite.AppendBool( true );
	bufWrite.AppendUint32( 0x12345678 );
	bufWrite.AppendInt32( -7 ); // signed 32-bit int
	bufWrite.AppendFloat( 1.f ); // in memory looks like hex: 00 00 80 3F (or 3F 80 00 00 in big endian)
	bufWrite.AppendDouble( 3.1415926535897932384626433832795 ); // actually 3.1415926535897931 (best it can do)
	bufWrite.AppendStringZeroTerminated( "Hello" ); // written with a trailing 0 ('\0') after (6 bytes total)
	bufWrite.AppendStringAfter32BitLength( "Is this thing on?" ); // uint 17, then 17 chars (no zero-terminator after)
	bufWrite.AppendRgba( Rgba8( 200, 100, 50, 255 ) ); // four bytes in RGBA order (endian-independent)
	bufWrite.AppendByte( 8 ); // 0x08 == 8 (byte)
	bufWrite.AppendRgb( Rgba8( 238, 221, 204, 255 ) ); // written as 3 bytes (RGB) only; ignores Alpha
	bufWrite.AppendByte( 9 ); // 0x09 == 9 (byte)
	bufWrite.AppendIntVec2( IntVec2( 1920, 1080 ) );
	bufWrite.AppendVec2( Vec2( -0.6f, 0.8f ) );
	bufWrite.AppendVertexPCU( Vertex_PCU( Vec3(3.f, 4.f, 5.f), Rgba8(100,101,102,103), Vec2(0.125f, 0.625f) ) );
}


//------------------------------------------------------------------------------------------------
void ParseTestFileBufferData( BufferParser& bufParse, EndianMode endianMode )
{
	// Parse known test file elements
	bufParse.SetEndianMode( endianMode );
	char fourCC0_T			= bufParse.ParseChar(); // 'T' == 0x54 hex == 84 decimal
	char fourCC1_E			= bufParse.ParseChar(); // 'E' == 0x45 hex == 84 decimal
	char fourCC2_S			= bufParse.ParseChar(); // 'S' == 0x53 hex == 69 decimal
	char fourCC3_T			= bufParse.ParseChar(); // 'T' == 0x54 hex == 84 decimal
	unsigned char version	= bufParse.ParseByte(); // version 2
	EndianMode mode		= (EndianMode) bufParse.ParseByte(); // 1 for little endian, or 2 for big endian
	bool shouldBeFalse		= bufParse.ParseBool(); // written in buffer as byte 0 or 1
	bool shouldBeTrue		= bufParse.ParseBool(); // written in buffer as byte 0 or 1
	unsigned int largeUint	= bufParse.ParseUint32(); // 0x12345678
	int negativeSeven		= bufParse.ParseInt32(); // -7 (as signed 32-bit int)
	float oneF				= bufParse.ParseFloat(); // 1.0f
	double pi				= bufParse.ParseDouble(); // 3.1415926535897932384626433832795 (or as best it can)

	std::string helloString, isThisThingOnString;
	bufParse.ParseStringZeroTerminated( helloString ); // written with a trailing 0 ('\0') after (6 bytes total)
	bufParse.ParseStringAfter32BitLength( isThisThingOnString ); // written as uint 17, then 17 characters (no zero-terminator after)

	Rgba8 rustColor			= bufParse.ParseRgba(); // Rgba8( 200, 100, 50, 255 )
	unsigned char eight		= bufParse.ParseByte(); // 0x08 == 8 (byte)
	Rgba8 seashellColor		= bufParse.ParseRgb(); // Rgba8( 238, 221, 204) written as 3 bytes (RGB) only; assume alpha is 255
	unsigned char nine		= bufParse.ParseByte(); // 0x09 == 9 (byte)
	IntVec2 highDefRes	= bufParse.ParseIntVec2(); // IntVector2( 1920, 1080 )
	Vec2 normal2D		= bufParse.ParseVec2(); // Vector2( -0.6f, 0.8f )
	Vertex_PCU vertex		= bufParse.ParseVertexPCU(); // VertexPCU( 3.f, 4.f, 5.f, Rgba(100,101,102,103), 0.125f, 0.625f ) );

	// Validate actual values parsed
	GUARANTEE_RECOVERABLE(fourCC0_T == 'T', "1");
	GUARANTEE_RECOVERABLE(fourCC1_E == 'E', "2");
	GUARANTEE_RECOVERABLE(fourCC2_S == 'S', "3");
	GUARANTEE_RECOVERABLE(fourCC3_T == 'T', "4");
	GUARANTEE_RECOVERABLE(version == 2, "5");
	GUARANTEE_RECOVERABLE(mode == endianMode, "6"); // verify that we're receiving things in the endianness we expect
	GUARANTEE_RECOVERABLE(shouldBeFalse == false, "7");
	GUARANTEE_RECOVERABLE(shouldBeTrue == true, "8");
	GUARANTEE_RECOVERABLE(largeUint == 0x12345678, "9");
	GUARANTEE_RECOVERABLE(negativeSeven == -7, "10");
	GUARANTEE_RECOVERABLE(oneF == 1.f, "11");
	GUARANTEE_RECOVERABLE(pi == 3.1415926535897932384626433832795, "12");
	GUARANTEE_RECOVERABLE(helloString == "Hello", "13");
	GUARANTEE_RECOVERABLE(isThisThingOnString == "Is this thing on?", "14");
	GUARANTEE_RECOVERABLE(rustColor == Rgba8(200, 100, 50, 255), "15");
	GUARANTEE_RECOVERABLE(eight == 8, "16");
	GUARANTEE_RECOVERABLE(seashellColor == Rgba8(238, 221, 204), "17");
	GUARANTEE_RECOVERABLE(nine == 9, "18");
	GUARANTEE_RECOVERABLE(highDefRes == IntVec2(1920, 1080), "19");
	GUARANTEE_RECOVERABLE(normal2D == Vec2(-0.6f, 0.8f), "20");
	GUARANTEE_RECOVERABLE(vertex.m_position == Vec3(3.f, 4.f, 5.f), "21");
	GUARANTEE_RECOVERABLE(vertex.m_color == Rgba8(100, 101, 102, 103), "22");
	GUARANTEE_RECOVERABLE(vertex.m_uvTexCoords == Vec2(0.125f, 0.625f), "23");
}

