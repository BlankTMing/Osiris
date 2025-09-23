#pragma once

#include <cstring>

#include <CS2/Classes/Entities/CEntityInstance.h>
#include <CS2/Classes/EntitySystem/CConcreteEntityList.h>
#include <CS2/Classes/EntitySystem/CEntityClass.h>
#include <CS2/Classes/EntitySystem/CEntityHandle.h>
#include <CS2/Classes/EntitySystem/CEntityIdentity.h>
#include <CS2/Classes/EntitySystem/CEntityIndex.h>
#include <CS2/Classes/EntitySystem/CGameEntitySystem.h>
#include <GameClient/EntitySystem/EntityIdentity.h>
#include <MemoryPatterns/PatternTypes/EntitySystemPatternTypes.h>

template <typename HookContext>
class EntitySystem {
public:
    explicit EntitySystem(HookContext& hookContext) noexcept
        : hookContext{hookContext}
    {
    }

    [[nodiscard]] decltype(auto) getEntityIdentityFromHandle(cs2::CEntityHandle handle) const noexcept
    {
        return hookContext.template make<EntityIdentity>(getRawEntityIdentityFromHandle(handle));
    }

    [[nodiscard]] cs2::CEntityInstance* getEntityFromHandle(cs2::CEntityHandle handle) const noexcept
    {
        if (const auto entityIdentity = getRawEntityIdentityFromHandle(handle))
            return entityIdentity->entity;
        return nullptr;
    }

    [[nodiscard]] decltype(auto) getEntityFromHandle2(cs2::CEntityHandle handle) const noexcept
    {
        return hookContext.template make<BaseEntity>(static_cast<cs2::C_BaseEntity*>(getEntityFromHandle(handle)));
    }
    
    template <typename F>
    void forEachEntityIdentity(F&& f) const noexcept
    {
        const auto entityList = getEntityList();
        if (!entityList)
            return;

        const auto highestEntityIndex = getHighestEntityIndex();
        for (int i = 0; i <= highestEntityIndex.value; ++i) {
            const auto chunkIndex = i / cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            const auto* const chunk = entityList->chunks[chunkIndex];
            if (!chunk)
                continue;

            const auto indexInChunk = i % cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            if (const auto& entityIndentity = (*chunk)[indexInChunk]; entityIndentity.entity && entityIndentity.handle.index().value == i)
                f(entityIndentity);
        }
    }

    [[nodiscard]] cs2::CEntityClass* findEntityClass(const char* className) const noexcept
    {
        const auto entityClasses = getEntityClasses();
        if (!entityClasses)
            return nullptr;

        for (int i = 0; i < entityClasses->numElements; ++i) {
            if (std::strcmp(entityClasses->memory[i].key, className) == 0)
                return entityClasses->memory[i].value;
        }
        return nullptr;
    }

private:
    [[nodiscard]] cs2::CEntityIdentity* getRawEntityIdentityFromHandle(cs2::CEntityHandle handle) const noexcept
    {
        const auto entityIndex = handle.index();
        if (!entityIndex.isValid())
            return nullptr;

        const auto entityList = getEntityList();
        if (!entityList)
            return nullptr;

        const auto chunkIndex = entityIndex.value / cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
        if (auto* const chunk = entityList->chunks[chunkIndex]) {
            const auto indexInChunk = entityIndex.value % cs2::CConcreteEntityList::kNumberOfIdentitiesPerChunk;
            if (auto& entityIndentity = (*chunk)[indexInChunk]; entityIndentity.handle == handle)
                return &entityIndentity;
        }
        return nullptr;
    }

    [[nodiscard]] cs2::CGameEntitySystem* entitySystem() const noexcept
    {
        if (hookContext.patternSearchResults().template get<EntitySystemPointer>())
            return *hookContext.patternSearchResults().template get<EntitySystemPointer>();
        return nullptr;
    }

    [[nodiscard]] auto getEntityList() const noexcept
    {
        return hookContext.patternSearchResults().template get<EntityListOffset>().of(entitySystem()).get();
    }

    [[nodiscard]] auto getEntityClasses() const noexcept
    {
        return hookContext.patternSearchResults().template get<OffsetToEntityClasses>().of(entitySystem()).get();
    }

    [[nodiscard]] auto getHighestEntityIndex() const noexcept
    {
        // fixme: highest entity index was removed from entity system in the 28 July 2025 update
        constexpr cs2::CEntityIndex kMaxNetworkableEntityIndex{16383};
        return kMaxNetworkableEntityIndex;
    }

    HookContext& hookContext;
};
