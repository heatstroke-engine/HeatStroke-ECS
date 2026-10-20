#pragma once

#include <vector>
#include <iostream>
#include <type_traits>
#include <utility>
#include <componentstorage.hpp>
#include <log.hpp>
#include <format>
#include <optional>

namespace HeatStroke
{
    /**
     * @brief Creates and manages entities, their components and tags, and the caches used by the systems.
     *
     * Entities are identified by a Key. The Entity objects themselves live inside the manager and are
     * accessed through their Key, so a Key stays valid while its entity exists, even if the internal
     * storage grows or moves data.
     *
     * @tparam CMPS     MP::Typelist with every component type the manager can store.
     * @tparam TAGS     MP::Typelist with every tag type the manager can assign.
     * @tparam Capacity Initial capacity of the entity and component slotmaps. They grow when full.
     */
    template<typename CMPS, typename TAGS, std::size_t Capacity = 1000>
    class EntityManager
    {
        public:
            /// @brief Storage holding one slotmap per component type.
            using cmp_storage_t = ComponentStorage<CMPS,TAGS,Capacity>;
            /// @brief Traits of the component list: ids and masks of each component.
            using cmpinfo = ComponentStorage<CMPS,TAGS,Capacity>::cmpinfo;
            /// @brief Traits of the tag list: ids and masks of each tag.
            using taginfo = ComponentStorage<CMPS,TAGS,Capacity>::taginfo;
            /// @brief Key type of the slotmap that stores components of type T.
            template<typename T>
            using cmp_key = HeatStroke::SlotMap<T,Capacity>::key_type;
            /// @brief Slotmap type that stores components of type CMP.
            template<typename CMP>
            using Slotmap_t = HeatStroke::SlotMap<CMP,Capacity>;

            /**
             * @brief An entity of the ECS: its id, the masks of its components and tags, and the keys of its components.
             *
             * The entity does not store the components, only the key of each one in its slotmap.
             * Only the EntityManager can modify the masks and keys, so they always match the component storage.
             */
            struct Entity
            {
                friend class EntityManager;

                /// @brief One key per component type, indexed by the component id.
                using Key_Storage_t = std::array<Key, cmpinfo::size()>;

                Entity() = default;

                /**
                 * @brief Id of the entity.
                 * @return Key of the entity in the manager's entity slotmap.
                 */
                Key getId() const;

                /**
                 * @brief Checks whether the entity has a component.
                 * @tparam CMP Component type to check. Must be part of CMPS.
                 * @return true if the entity has a CMP component.
                 */
                template<typename CMP>
                bool hasComponent() const noexcept
                {
                    return (cmpinfo::template mask<CMP>() & cmp_mask);
                }

                /**
                 * @brief Checks whether the entity has a tag.
                 * @tparam TAG Tag type to check. Must be part of TAGS.
                 * @return true if the entity has the TAG tag.
                 */
                template<typename TAG>
                bool hasTag() const noexcept
                {
                    return (taginfo::template mask<TAG>() & tag_mask);
                }

                /**
                 * @brief Key of one of the entity's components in its slotmap.
                 * @tparam CMP Component type. Must be part of CMPS.
                 * @return Key of the component.
                 * @note Only meaningful if the entity has the component (see hasComponent).
                 */
                template<typename CMP>
                cmp_key<CMP> getComponentKey() const
                {
                    return keys[cmpinfo::template id<CMP>()];
                }

                /// @brief Mask with one bit set for each component the entity has.
                inline cmpinfo::mask_type getCMPmask() const noexcept
                {
                    return cmp_mask;
                }

                /// @brief Mask with one bit set for each tag the entity has.
                inline taginfo::mask_type getTAGmask() const noexcept
                {
                    return tag_mask;
                }

                private:
                    /**
                     * @brief Marks the entity as having a CMP component and stores its key.
                     * @tparam CMP Component type.
                     * @param key Key of the component in its slotmap.
                     */
                    template<typename CMP>
                    void addComponent(cmp_key<CMP> key)
                    {
                        cmp_mask |= cmpinfo::template mask<CMP>();

                        keys[cmpinfo::template id<CMP>()] = key;
                    }

                    /**
                     * @brief Clears the bit of CMP in the component mask.
                     * @tparam CMP Component type.
                     */
                    template<typename CMP>
                    void removeComponent()
                    {
                        cmp_mask &= ~cmpinfo::template mask<CMP>();
                    }

                    /**
                     * @brief Sets the bit of TAG in the tag mask.
                     * @tparam TAG Tag type.
                     */
                    template<typename TAG>
                    void addTag()
                    {
                        tag_mask |= taginfo::template mask<TAG>();
                    }

                    /**
                     * @brief Clears the bit of TAG in the tag mask.
                     * @tparam TAG Tag type.
                     */
                    template<typename TAG>
                    void removeTag()
                    {
                        tag_mask &= ~taginfo::template mask<TAG>();
                    }

                    cmpinfo::mask_type cmp_mask {};     ///< One bit per component the entity has.
                    taginfo::mask_type tag_mask {};     ///< One bit per tag the entity has.
                    Key id{};                           ///< Key of the entity in the manager's entity slotmap.
                    Key_Storage_t keys {};              ///< Key of each component in its slotmap, indexed by component id.

            };

            /**
             * @brief Precached list of the entities that match a system signature.
             *
             * An entity matches when it has every component of cmp_mask and every tag of tag_mask.
             * Systems with the same signature share the same cache.
             */
            struct SystemCache
            {
                cmpinfo::mask_type cmp_mask;    ///< Components required by the signature.
                taginfo::mask_type tag_mask;    ///< Tags required by the signature.

                std::vector<Key> entities{};    ///< Keys of the entities that match the signature.
            };

            /**
             * @brief Creates a new entity without components or tags.
             * @return Key of the new entity.
             */
            Key createEntity();

            /**
             * @brief Removes an entity and all its components from the manager.
             * @param entityId Key of the entity to remove.
             * @note If the key is not valid, logs a warning and does nothing.
             */
            void removeEntity(Key entityId);

            /**
             * @brief Adds a component to an entity.
             * @tparam CMP Component type, deduced from the argument. Must be part of CMPS.
             * @param eID Key of the entity.
             * @param cmp Component to add. It is copied or moved depending on its value category.
             * @return Reference to the stored component.
             * @warning The reference is only valid until the next structural change in the storage of that
             *          component (adding or removing a component of the same type). Do not store it.
             * @pre The entity must be valid and must not already have a CMP component.
             */
            template<typename CMP>
            std::remove_cvref_t<CMP>& addComponent(Key eID, CMP&& cmp) noexcept;

            /**
             * @brief Adds several components to an entity.
             * @tparam CMPs Component types, deduced from the arguments. Must be part of CMPS.
             * @param eID Key of the entity.
             * @param cmp Components to add. Each one is copied or moved depending on its value category.
             * @pre The entity must be valid and must not already have any of the components.
             */
            template<typename... CMPs>
            void addComponents(Key eID, CMPs&&... cmp) noexcept;

            /**
             * @brief Removes a component from an entity.
             * @tparam CMP Component type to remove.
             * @param eID Key of the entity.
             * @pre The entity must be valid and must have a CMP component.
             */
            template<typename CMP>
            void removeComponent(Key eID) noexcept;

            /**
             * @brief Removes several components from an entity.
             * @tparam CMPs Component types to remove.
             * @param eID Key of the entity.
             * @pre The entity must be valid and must have every component in CMPs.
             */
            template<typename... CMPs>
            void removeComponents(Key eID) noexcept;

            /**
             * @brief Adds a tag to an entity.
             * @tparam TAG Tag type. Must be part of TAGS.
             * @param eID Key of the entity.
             * @pre The entity must be valid and must not already have the tag.
             */
            template<typename TAG>
            void addTag(Key eID) noexcept;

            /**
             * @brief Adds several tags to an entity.
             * @tparam TAGs Tag types. Must be part of TAGS.
             * @param eID Key of the entity.
             * @pre The entity must be valid and must not already have any of the tags.
             */
            template<typename... TAGs>
            void addTags(Key eID) noexcept;

            /**
             * @brief Removes a tag from an entity.
             * @tparam TAG Tag type.
             * @param eID Key of the entity.
             * @pre The entity must be valid and must have the tag.
             */
            template<typename TAG>
            void removeTag(Key eID) noexcept;

            /**
             * @brief Removes several tags from an entity.
             * @tparam TAGs Tag types.
             * @param eID Key of the entity.
             * @pre The entity must be valid and must have every tag in TAGs.
             */
            template<typename... TAGs>
            void removeTags(Key eID) noexcept;

            /**
             * @brief Gets a component of an entity.
             * @tparam CMP Component type.
             * @param eID Key of the entity.
             * @return Reference to the component.
             * @warning The reference is only valid until the next structural change in the storage of that
             *          component. Do not store it.
             * @pre The entity must be valid and must have a CMP component.
             */
            template<typename CMP>
            CMP& getComponent(const Key eID) noexcept;

            /**
             * @brief Gets a component of an entity (const version).
             * @tparam CMP Component type.
             * @param eID Key of the entity.
             * @return Const reference to the component.
             * @warning The reference is only valid until the next structural change in the storage of that
             *          component. Do not store it.
             * @pre The entity must be valid and must have a CMP component.
             */
            template<typename CMP>
            const CMP& getComponent(const Key eID) const noexcept;

            /**
             * @brief Gets an entity from its key.
             * @param id Key of the entity.
             * @return Pointer to the entity, or nullptr if the key is not valid.
             * @warning The pointer is invalidated when entities are created or removed. Do not store it,
             *          store the key instead.
             */
            Entity* getEntityById(const Key id)noexcept;

            /**
             * @brief Registers a system and links it to the cache of its signature.
             *
             * Computes the component and tag masks of the system signature at compile time. If a cache
             * with the same signature already exists, the system reuses it; otherwise a new cache is
             * created and filled with the existing entities that match the signature.
             *
             * @tparam SYS System type. Must define `SCMPs` and `STAGs` (MP::Typelist with the components
             *             and tags it requires) and a static `cache_index`, where the index of its cache is stored.
             * @note Fails to compile if the signature contains a component or tag that is not part of CMPS or TAGS.
             */
            template<typename SYS>
            void registerSystem() noexcept;

            /**
             * @brief Calls a function for every entity that matches the signature of a system.
             *
             * Iterates the cache of SYS and, for each entity, calls `func(Key, CMPs&...)`, where CMPs are
             * the components of `SYS::SCMPs` in the same order they are declared there. For example, with
             * `SCMPs = MP::Typelist<CPhysics, CTransform>` the expected function is
             * `func(Key, CPhysics&, CTransform&)` (components can also be taken as const references).
             *
             * @tparam SYS System type. Must have been registered with registerSystem.
             * @param func Function to call for each entity of the system cache.
             * @pre SYS must have been registered.
             * @warning Do not make structural changes inside func (create or remove entities, add or remove
             *          components or tags): they can invalidate the cache being iterated.
             */
            template<typename SYS>
            void foreach(auto&& func) noexcept;
            

            /// @brief Creates an empty manager.
            EntityManager();
            // Deleted functions
            EntityManager(const EntityManager& rhs) = delete;
            EntityManager& operator=(const EntityManager& rhs) = delete;
            private:

                /**
                 * @brief Gets a component directly from an entity, without looking up the entity or checking it has the component.
                 * @tparam CMP Component type.
                 * @param e Entity that owns the component.
                 * @return Reference to the component.
                 * @pre e must have a CMP component (guaranteed for the entities of a matching cache).
                 */
                template<typename CMP>
                CMP& getComponent(const Entity& e) noexcept
                {
                    cmp_key<CMP> key = e.template getComponentKey<CMP>();
                    Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();

                    return storage[key];
                }
                /**
                 * @brief Removes a component from an entity if the entity has it.
                 * @tparam CMP Component type.
                 * @param e Entity to remove the component from.
                 */
                template<typename CMP>
                void removeIfPresent(Entity& e) noexcept
                {
                    if (e.template hasComponent<CMP>())
                    {
                            removeComponent_impl<CMP>(e);
                    }
                }


                /**
                 * @brief Removes every component of an entity.
                 * @tparam I Index of each component type in CMPS (0, 1, ..., N-1).
                 * @param e Entity to remove the components from.
                 */
                template<std::size_t... I>
                void removeEntityimp(Entity& e, std::index_sequence<I...>)noexcept
                {
                    //Updata caches
                    for(SystemCache& cache : caches_)
                    {
                         const bool contains = matches(cache, e.getCMPmask(), e.getTAGmask());
                         if(contains)
                         {
                             std::erase(cache.entities, e.getId());
                         }
                    }
                   (removeIfPresent<typename cmpinfo::template at<I> >(e), ...);
                }

                /**
                 * @brief Implementation of foreach: unpacks the component types of the system signature.
                 * @tparam CMPs Components of the system signature, deduced from the Typelist argument.
                 * @param func Function to call for each entity, as `func(Key, CMPs&...)`.
                 * @param precached Cache of the system to iterate.
                 */
                template<typename... CMPs>
                void foreach_impl(auto&& func, const SystemCache& precached, MP::Typelist<CMPs...>)
                {
                    for(const Key eID : precached.entities)
                            {
                                Entity& e = entities_[eID];
                                func(eID, getComponent<CMPs>(e)...);
                            }
                }

                /**
                 * @brief Adds a component to an entity without updating the caches.
                 * @tparam CMP Component type, deduced from the argument.
                 * @param e Entity to add the component to.
                 * @param cmp Component to add. It is copied or moved depending on its value category.
                 * @return Reference to the stored component.
                 * @pre e must not already have a CMP component. The caller must call updateCacheFor afterwards.
                 */
                template<typename CMP>
                std::remove_cvref_t<CMP>& addComponent_impl(Entity& e, CMP&& cmp) noexcept;

                /**
                 * @brief Adds a tag to an entity without updating the caches.
                 * @tparam TAG Tag type.
                 * @param e Entity to add the tag to.
                 * @pre e must not already have the tag. The caller must call updateCacheFor afterwards.
                 */
                template<typename TAG>
                void addTag_impl(Entity& e) noexcept;

                /**
                 * @brief Removes a component from an entity without updating the caches.
                 * @tparam CMP Component type.
                 * @param e Entity to remove the component from.
                 * @pre e must have a CMP component. The caller must call updateCacheFor afterwards.
                 */
                template<typename CMP>
                void removeComponent_impl(Entity& e) noexcept;

                /**
                 * @brief Removes a tag from an entity without updating the caches.
                 * @tparam TAG Tag type.
                 * @param e Entity to remove the tag from.
                 * @pre e must have the tag. The caller must call updateCacheFor afterwards.
                 */
                template<typename TAG>
                void removeTag_impl(Entity& e) noexcept;

                /**
                 * @brief Checks if a pair of masks matches the signature of a cache.
                 * @param cache Cache to check against.
                 * @param cmp_mask Component mask of the entity.
                 * @param tag_mask Tag mask of the entity.
                 * @return true if the masks contain every component and tag of the cache signature.
                 */
                static constexpr bool matches(const SystemCache& cache, typename cmpinfo::mask_type cmp_mask,
                                              typename taginfo::mask_type tag_mask) noexcept
                {
                    return ((cmp_mask & cache.cmp_mask) == cache.cmp_mask) &&
                           ((tag_mask & cache.tag_mask) == cache.tag_mask);
                }

                /**
                 * @brief Adds or removes an entity from the caches after a structural change.
                 *
                 * Compares the masks the entity had before the change with the current ones, so it knows
                 * if the entity was in each cache without searching it.
                 * @param e Entity that changed.
                 * @param old_cmp_mask Component mask of the entity before the change.
                 * @param old_tag_mask Tag mask of the entity before the change.
                 */
                void updateCacheFor(const Entity& e, typename cmpinfo::mask_type old_cmp_mask,
                                    typename taginfo::mask_type old_tag_mask) noexcept
                {
                    for(SystemCache& cache : caches_)
                    {
                        const bool was_in = matches(cache, old_cmp_mask, old_tag_mask);
                        const bool is_in  = matches(cache, e.getCMPmask(), e.getTAGmask());

                        if(is_in && !was_in)
                        {
                            cache.entities.push_back(e.getId());
                        }else if(was_in && !is_in)
                        {
                            std::erase(cache.entities, e.getId());
                        }
                    }
                }


                SlotMap<Entity, Capacity> entities_{};  ///< Every entity of the manager.
                cmp_storage_t components_{};            ///< One slotmap per component type.

                /// @brief Container of the system caches.
                using SystemCacheContainer_t = std::vector<SystemCache>;

                SystemCacheContainer_t caches_{};       ///< One cache per distinct system signature.
    };


    template<typename CMPS, typename TAGS, std::size_t Capacity>
    inline Key EntityManager<CMPS, TAGS, Capacity>::createEntity()
    {
        // Push back an empty Entity
        Key key = entities_.push_back(Entity{});

        // Set its id
        Entity& e = entities_[key];
        e.id = key;
        // Return the Entity
        return key;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    void EntityManager<CMPS, TAGS, Capacity>::removeEntity(Key entityId)
    {
        if(!entities_.is_valid(entityId))
        {
            warning("Invalid Entity id.");
            return;
        }

        removeEntityimp(entities_[entityId], std::make_index_sequence<cmpinfo::list_size>{});
        entities_.erase(entityId);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    Key EntityManager<CMPS, TAGS, Capacity>::Entity::getId() const
    {
        return id;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    EntityManager<CMPS, TAGS, Capacity>::EntityManager() :
        entities_(),
        components_(),
        caches_()
    {

    }


    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... CMPs>
    void EntityManager<CMPS, TAGS, Capacity>::addComponents(Key eID, CMPs&&... cmp) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        (addComponent_impl(e, std::forward<CMPs>(cmp)), ...);

        // Update the caches only once, with the final masks
        updateCacheFor(e, old_cmp_mask, old_tag_mask);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    std::remove_cvref_t<CMP>& EntityManager<CMPS, TAGS, Capacity>::addComponent(Key eID, CMP&& cmp) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        auto& component = addComponent_impl(e, std::forward<CMP>(cmp));

        updateCacheFor(e, old_cmp_mask, old_tag_mask);

        return component;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    std::remove_cvref_t<CMP>& EntityManager<CMPS, TAGS, Capacity>::addComponent_impl(Entity& e, CMP&& cmp) noexcept
    {
        using T = std::remove_cvref_t<CMP>;

        Slotmap_t<T>& storage = components_.template getStorage<T>();
        cmp_key<T> key;

        debugAssertion(!e.template hasComponent<T>(), "Trying to add a component to an Entity that already have it.");

        // Add component to the slotmap (copied or moved depending on the value category)
        key = storage.push_back(std::forward<CMP>(cmp));
        //Add key to the Entity
        e.template addComponent<T>(key);

        return storage[key];
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    void EntityManager<CMPS, TAGS, Capacity>::removeComponent(Key eID) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        removeComponent_impl<CMP>(e);

        updateCacheFor(e, old_cmp_mask, old_tag_mask);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... CMPs>
    void EntityManager<CMPS, TAGS, Capacity>::removeComponents(Key eID) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        (removeComponent_impl<CMPs>(e), ...);

        // Update the caches only once, with the final masks
        updateCacheFor(e, old_cmp_mask, old_tag_mask);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    void EntityManager<CMPS, TAGS, Capacity>::removeComponent_impl(Entity& e) noexcept
    {
        debugAssertion(e.template hasComponent<CMP>(), "Trying to remove a component from an Entity that doesn`t have it.");

        Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();
        cmp_key<CMP> key = e.template getComponentKey<CMP>();

        // Remove CMP from the slotmap
        storage.erase(key);

        e.template removeComponent<CMP>();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename TAG>
    void EntityManager<CMPS, TAGS, Capacity>::addTag(Key eID) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        addTag_impl<TAG>(e);

        updateCacheFor(e, old_cmp_mask, old_tag_mask);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... TAGs>
    void EntityManager<CMPS, TAGS, Capacity>::addTags(Key eID) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        (addTag_impl<TAGs>(e), ...);

        // Update the caches only once, with the final masks
        updateCacheFor(e, old_cmp_mask, old_tag_mask);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename TAG>
    void EntityManager<CMPS, TAGS, Capacity>::addTag_impl(Entity& e) noexcept
    {
        debugAssertion(!e.template hasTag<TAG>(), "Trying to add a tag from an Entity that already have it.");
        e.template addTag<TAG>();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename TAG>
    void EntityManager<CMPS, TAGS, Capacity>::removeTag(Key eID) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        removeTag_impl<TAG>(e);

        updateCacheFor(e, old_cmp_mask, old_tag_mask);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... TAGs>
    void EntityManager<CMPS, TAGS, Capacity>::removeTags(Key eID) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        Entity& e = entities_[eID];

        // Save the masks before the change
        const auto old_cmp_mask = e.getCMPmask();
        const auto old_tag_mask = e.getTAGmask();

        (removeTag_impl<TAGs>(e), ...);

        // Update the caches only once, with the final masks
        updateCacheFor(e, old_cmp_mask, old_tag_mask);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename TAG>
    void EntityManager<CMPS, TAGS, Capacity>::removeTag_impl(Entity& e) noexcept
    {
        debugAssertion(e.template hasTag<TAG>(), "Trying to remove a tag from an Entity that doesn`t have it.");
        e.template removeTag<TAG>();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    CMP& EntityManager<CMPS, TAGS, Capacity>::getComponent(const Key eID) noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        const Entity& e = entities_[eID];
        debugAssertion(e.template hasComponent<CMP>(), "Trying to get a component from an Entity that doesn`t have it.");

        cmp_key<CMP> key = e.template getComponentKey<CMP>();
        Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();

        return storage[key];
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    const CMP& EntityManager<CMPS, TAGS, Capacity>::getComponent(const Key eID) const noexcept
    {
        debugAssertion(entities_.is_valid(eID), "Invalid Key.");
        const Entity& e = entities_[eID];
        debugAssertion(e.template hasComponent<CMP>(), "Trying to get a component from an Entity that doesn`t have it.");

        cmp_key<CMP> key = e.template getComponentKey<CMP>();
        const Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();

        return storage[key];
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    typename EntityManager<CMPS, TAGS, Capacity>::Entity* EntityManager<CMPS, TAGS, Capacity>::getEntityById(const Key id) noexcept
    {

        return entities_.is_valid(id) ? &entities_[id] : nullptr;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename SYS>
    void EntityManager<CMPS, TAGS, Capacity>::registerSystem() noexcept
    {
        using SystemCMPs = typename SYS::SCMPs;
        using SystemTAGs = typename SYS::STAGs;

        compileTimeAssertion(CMPS::template includes(SystemCMPs{}) , "System signature contains a nonpresent CMP type");
        compileTimeAssertion(TAGS::template includes(SystemTAGs{}), "System signature contains a nonpresent TAG type");
        // Compute system masks
        constexpr typename cmpinfo::mask_type cmp_mask0 = cmpinfo::template mask(SystemCMPs{});
        constexpr typename taginfo::mask_type tag_mask0 = taginfo::template mask(SystemTAGs{});

        logPrint(std::format("CMP mask {:#b}", cmp_mask0));
        logPrint(std::format("TAG mask {:#b}", tag_mask0));


        // Checks is there is a chache that already mach the signature
        bool finded = false;
        std::size_t cache_index = 0;

        for(std::size_t i{}; !finded && i < caches_.size(); ++i)
        {
            const SystemCache& cache = caches_[i];

            const bool mached_cmp_mask = (cache.cmp_mask  == cmp_mask0);
            const bool mached_tag_mask = (cache.tag_mask  == tag_mask0);
            if(mached_cmp_mask && mached_tag_mask )
            {
                cache_index = i;
                finded = true;
            }
        }

        if(!finded)
        {
            // Add the cache to cache_
            SystemCache& cache = caches_.emplace_back();
            cache_index = caches_.size() - 1;

            cache.cmp_mask = cmp_mask0;
            cache.tag_mask = tag_mask0;

            for(const Entity& e : entities_)
            {
                const bool contains_cmps = ((e.getCMPmask() & cache.cmp_mask) == cache.cmp_mask);
                const bool contains_tags = ((e.getTAGmask() & cache.tag_mask) == cache.tag_mask);
                if(contains_cmps && contains_tags)
                {
                    cache.entities.push_back(e.getId());
                    logPrint("Entity with id: " + std::to_string(e.getId().gen) + " added to cache.");
                }
            }
        }
        // Update the system index
        SYS::cache_index = cache_index;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename SYS>
    void EntityManager<CMPS, TAGS, Capacity>::foreach(auto&& func) noexcept
    {        
        debugAssertion(SYS::cache_index < caches_.size(), "System not registered");
        const SystemCache& precached = caches_[SYS::cache_index];
        debugAssertion(precached.cmp_mask == cmpinfo::mask(typename SYS::SCMPs{}) &&
                    precached.tag_mask == taginfo::mask(typename SYS::STAGs{}),
                    "System not registered or wrong cache");

        foreach_impl(func, precached, typename SYS::SCMPs{});
        




    }

} // namespace HeatStroke
