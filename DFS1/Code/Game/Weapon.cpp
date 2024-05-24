#include "Game/Weapon.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/AI.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/Window.hpp"

Weapon::Weapon(const char* name, const Clock* clock)
{
    m_weaponDef = WeaponDefinition::GetWeaponDef(name);
    m_refireTime = Stopwatch(clock, m_weaponDef->m_refireTime);
    m_animeClock = Stopwatch(clock, 0.f);
    m_refireTime.Start();
}

void Weapon::Fire(Actor* owner)
{
    if (!m_refireTime.HasDurationElapsed()) {
        if (m_refireTime.m_startTime == 0.f) {
            m_refireTime.Start();
        }
        else
        {
            return;
        }
    }
    else {
        m_refireTime.Restart();
    }
    Vec3 firePoint = owner->m_position;
    firePoint.z += owner->m_actorDef->m_eyeHeight;
    if (owner->m_isRiding && owner->m_ridingActor) {
        firePoint.z += owner->m_ridingActor->m_actorDef->m_height;
    }
    SetNowAnimationByName("Attack");
    if (m_weaponDef->m_audios.find("Fire") != m_weaponDef->m_audios.end())
        PlayMusic(m_weaponDef->m_audios.at("Fire"), owner->m_position);
    if (m_weaponDef->m_name == "Pistol") {
        Actor* target = nullptr;
        RaycastResult3D result = owner->m_map->RaycastAll(firePoint, owner->GetFwdNormal(), m_weaponDef->m_rayRange, target);
        if (target != nullptr) {
            int damage = g_rng->RollRandomIntInRange((int)m_weaponDef->m_rayDamage.m_min, (int)m_weaponDef->m_rayDamage.m_max);
            target->Damage(owner, damage);
            target->AddImpulse(-target->m_velocity.GetNormalized() * m_weaponDef->m_rayImpulse);
            // set ai target
            if (target->m_controller != nullptr && target->m_controller == target->m_AIController 
                && owner->m_actorDef->m_faction != target->m_actorDef->m_faction) {
                AI* ai = (AI*)target->m_controller;
                ai->m_targetUID = owner->m_UID;
            }
            SpawnInfo spawninfo;
            spawninfo.m_actor = "BloodSplatter";
            spawninfo.m_position = result.m_impactPos;
            owner->m_map->SpawnActor(spawninfo);
        }
        else {
            SpawnInfo spawninfo;
            spawninfo.m_actor = "BulletHit";
            spawninfo.m_position = result.m_impactPos;
            owner->m_map->SpawnActor(spawninfo);
        }
        
    }
    if (m_weaponDef->m_name == "PlasmaRifle") {
        SpawnInfo spawninfo;
        spawninfo.m_actor = m_weaponDef->m_projectileActor;
        Vec3 direction = GetRandomDirectionInCone(owner->m_orientation, m_weaponDef->m_projectileCone);
        spawninfo.m_position = firePoint; 
            //+ (owner->m_actorDef->m_radius + 0.02f) * direction.GetVec2XY().GetNormalized();
        spawninfo.m_velocity = m_weaponDef->m_projectileSpeed * direction;
        spawninfo.m_position.z -= owner->m_map->m_level;
        Actor* projectile = owner->m_map->SpawnActor(spawninfo);
        projectile->m_owner = owner;
    }
    if (m_weaponDef->m_name == "DemonMelee") {
        if (owner->m_controller != nullptr && owner->m_controller == owner->m_AIController) {
            AI* ai = (AI*)owner->m_controller;
            Actor* myTarget = ai->m_map->GetActorByUID(ai->m_targetUID);
            if (myTarget != nullptr && IsPointInsideDirectedSector2D(myTarget->m_position.GetVec2XY(), owner->m_position.GetVec2XY()
            , owner->GetFwdNormal().GetVec2XY(), m_weaponDef->m_meleeArc, m_weaponDef->m_meleeRange)) {
                int damage = g_rng->RollRandomIntInRange((int)m_weaponDef->m_meleeDamage.m_min, (int)m_weaponDef->m_meleeDamage.m_max);
                myTarget->Damage(owner, damage);
                myTarget->AddImpulse(-myTarget->m_velocity.GetNormalized() * m_weaponDef->m_meleeImpulse);
                owner->SetNowAnimationByName("Attack");
            }
        }
        
    }
    if (m_weaponDef->m_name == "Vacuum") {
        if (m_stateToggle) {
            SpawnInfo spawninfo;
            spawninfo.m_actor = m_vacuumProjectile;
                //m_weaponDef->m_projectileActor;
            Vec3 direction = GetRandomDirectionInCone(owner->m_orientation, m_weaponDef->m_projectileCone);
            spawninfo.m_position = firePoint;
 
            spawninfo.m_velocity = m_weaponDef->m_projectileSpeed * direction;
            spawninfo.m_orientation = m_owner->m_orientation;

            Actor* projectile = owner->m_map->SpawnActor(spawninfo);
            projectile->m_owner = owner;

            m_stateToggle = false;
        }
        else
        {
            Actor* target = nullptr;
            RaycastResult3D result = owner->m_map->RaycastAll(firePoint, owner->GetFwdNormal(), m_weaponDef->m_rayRange, target);
            if (target != nullptr) {
                if (target->m_actorDef->m_name == "Demon") {
                    m_vacuumProjectile = "DemonProjectile";
                }
                else if (target->m_actorDef->m_name == "LostSoul") {
                    m_vacuumProjectile = "LostSoulProjectile";
                }
                else
                {
                    return;
                }
                SpawnInfo spawninfo;
                spawninfo.m_actor = "VacuumHit";
                Vec3 position = target->m_position;
                position.z += 0.5f;
                spawninfo.m_position = position;


                owner->m_map->SpawnActor(spawninfo);
                target->m_isGarbage = true;

                m_stateToggle = true;
            }
        }
    }
    if (m_weaponDef->m_name == "SoulBomb") {
        for (int actorIndex = 0; actorIndex < (int)owner->m_map->m_actors.size(); actorIndex++) {
            Actor* const& actor = owner->m_map->m_actors[actorIndex];
            if (actor) {
                if (actor != owner) {
                    if (actor->m_actorDef->m_faction != owner->m_actorDef->m_faction) {
                        if (GetDistanceSquared2D(actor->m_position.GetVec2XY(), owner->m_position.GetVec2XY()) <= m_weaponDef->m_meleeRange * m_weaponDef->m_meleeRange) {
                            int damage = g_rng->RollRandomIntInRange((int)m_weaponDef->m_meleeDamage.m_min, (int)m_weaponDef->m_meleeDamage.m_max);
                            actor->Damage(owner, damage);
                            actor->AddImpulse((actor->m_position - owner->m_position).GetNormalized()* m_weaponDef->m_meleeImpulse);
                        }
                        
                    }
                }
            }
        }
        SpawnInfo spawninfo;
        spawninfo.m_actor = "Explosion";
        spawninfo.m_position = owner->m_position;
        spawninfo.m_position.z += 0.5f;
        owner->m_map->SpawnActor(spawninfo);
    }
}

Vec3 Weapon::GetRandomDirectionInCone(EulerAngles direction, float degree)
{
    float random = g_rng->RollRandomFloatInRange(0.f, 360.f);
    Vec3 origin = Vec3(CosDegrees(degree), SinDegrees(degree) * CosDegrees(random), SinDegrees(degree) * SinDegrees(random));
    Vec3 iBasis, jBasis, kBasis;
    direction.GetVectors_XFwd_YLeft_ZUp(iBasis, jBasis, kBasis);
    TransformPositionXYZ3D(origin, iBasis, jBasis, kBasis, Vec3());
    return origin;
}

void Weapon::RenderHUD(const Camera& camera)
{
    std::vector<Vertex_PCU> hudVerts, reticleVerts, weaponVerts;
    AABB2 hudAABB, reticleAABB, weaponAABB;
    AABB2 cameraFrame = 
        //AABB2(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
        AABB2(camera.m_bottomLeft, camera.m_topRight);
    hudAABB = AABB2(Vec2(0.f, 0.f), Vec2(cameraFrame.GetDimensions().x, 100.f));
    reticleAABB = AABB2();
    reticleAABB.SetCenter(Vec2(cameraFrame.GetDimensions().x * 0.5f, cameraFrame.GetDimensions().y * 0.5f));
    reticleAABB.SetDimensions(Vec2((float)m_weaponDef->m_reticleSize.x, (float)m_weaponDef->m_reticleSize.y));
    weaponAABB = AABB2();
    weaponAABB.SetCenter(Vec2(cameraFrame.GetDimensions().x * 0.5f, 100.f + 0.5f * (float)m_weaponDef->m_spriteSize.y));
    weaponAABB.SetDimensions(Vec2((float)m_weaponDef->m_spriteSize.x * (camera.m_viewport.GetDimensions().y / (float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y), (float)m_weaponDef->m_spriteSize.y));

    if (m_weaponDef->m_animGroups.size() > 0)
    {
        SpriteDefinition spriteDef = GetSpriteDefByName(m_nowAnimeName);
        AddVertsForAABB2D(weaponVerts, weaponAABB, Rgba8::WHITE, spriteDef.GetUVs().m_mins, spriteDef.GetUVs().m_maxs);
        g_theRenderer->BindTexture(&spriteDef.GetTexture());
        g_theRenderer->DrawVertexArray((int)weaponVerts.size(), weaponVerts.data());
        g_theRenderer->BindTexture(nullptr);
    }

    AddVertsForAABB2D(hudVerts, hudAABB, Rgba8::WHITE);
    AddVertsForAABB2D(reticleVerts, reticleAABB, Rgba8::WHITE);

    g_theRenderer->BindTexture(m_weaponDef->m_baseTexture);
    if (m_weaponDef->m_baseTexture)
        g_theRenderer->DrawVertexArray((int)hudVerts.size(), hudVerts.data());
    g_theRenderer->BindTexture(nullptr);

    g_theRenderer->BindTexture(m_weaponDef->m_reticleTexture);
    if (m_weaponDef->m_reticleTexture)
        g_theRenderer->DrawVertexArray((int)reticleVerts.size(), reticleVerts.data());
    g_theRenderer->BindTexture(nullptr);

    if (m_weaponDef->m_name == "Vacuum") {
        if (m_stateToggle) {
            std::vector<Vertex_PCU> projectileVerts;
            AABB2 projectile;
            projectile.SetDimensions(Vec2((float)m_weaponDef->m_projectileSize.x, (float)m_weaponDef->m_projectileSize.y));
            projectile.SetCenter(Vec2(cameraFrame.GetDimensions().x * 0.5f, (float)m_weaponDef->m_projectileSize.y * 0.5f));
            AddVertsForAABB2D(projectileVerts, projectile, Rgba8::WHITE);
            if (m_vacuumProjectile == "DemonProjectile") {
                g_theRenderer->BindTexture(m_weaponDef->m_demonProjectileTexture);
            }
            if (m_vacuumProjectile == "LostSoulProjectile") {
                g_theRenderer->BindTexture(m_weaponDef->m_soulProjectileTexture);
            }
            g_theRenderer->DrawVertexArray((int)projectileVerts.size(), projectileVerts.data());
            g_theRenderer->BindTexture(nullptr);
        }
    }
    

}

SpriteDefinition Weapon::GetSpriteDefByName(std::string name) const
{
    for (int animeIndex = 0; animeIndex < m_weaponDef->m_animGroups.size(); animeIndex++) {
        if (m_weaponDef->m_animGroups[animeIndex].m_name == name) {
            return m_weaponDef->m_animGroups[animeIndex].GetSpriteDefAtTimeAndDirection(m_animeClock.GetElapsedTime());
        }
    }
    return m_weaponDef->m_animGroups[0].GetSpriteDefAtTimeAndDirection(m_animeClock.GetElapsedTime());
}

SpriteAnimGroupDefinition Weapon::GetSpriteAnimGroupDefinitionByName(std::string name) const
{
    for (int groupIndex = 0; groupIndex < m_weaponDef->m_animGroups.size(); groupIndex++) {
        if (name == m_weaponDef->m_animGroups[groupIndex].m_name) {
            return m_weaponDef->m_animGroups[groupIndex];
        }
    }
    return SpriteAnimGroupDefinition();
}

void Weapon::RefreshAnimation()
{
    if (m_weaponDef->m_animGroups.size() <= 0)
        return;
    if (m_animeClock.HasDurationElapsed()) {
        SpriteAnimGroupDefinition nowGroup = GetSpriteAnimGroupDefinitionByName(m_nowAnimeName);
        if (nowGroup.m_playbackType == SpriteAnimPlaybackType::ONCE) {
            // change to default one
            m_nowAnimeName = m_weaponDef->m_animGroups[0].m_name;
            SetNowAnimationByName(m_nowAnimeName);
        }
    }
}

void Weapon::SetNowAnimationByName(std::string name)
{
    SpriteAnimGroupDefinition temp = GetSpriteAnimGroupDefinitionByName(name);
    m_nowAnimeName = name;
    m_animeClock.m_duration = temp.GetDuration();
    if (name == "Idle") {
        m_animeClock.Stop();
    }
    else {
        m_animeClock.Restart();
    }
}
