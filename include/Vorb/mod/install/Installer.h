//
// Installer.h
// Vorb Engine
//
// Created by Matthew Marshall on 16 July 2019
// Copyright 2019 Regrowth Studios
// MIT License
//

/*! \file Installer.h
 * \brief Holds all things needed for a given mod. This includes the mod's metadata, but also
 * basic utilities such as an IO manager, texture cache etc.
 */

#pragma once

#ifndef Vorb_Installer_h__
//! @cond DOXY_SHOW_HEADER_GUARDS
#define Vorb_Installer_h__
//! @endcond

#ifndef VORB_USING_PCH
#include "../types.h"
#endif // !VORB_USING_PCH

#include "Vorb/VorbPreDecl.inl"

DECL_VIO(class IOManager; class Path)

namespace vorb {
    namespace mod {
        namespace install {
            struct InstallStrategy;

            // TODO(Matthew): Provide registration functions that are used to specify
            //                YAML docs that mod can provide deltas to merge in.
            //                    Don't want to hardcode anything like this, though
            //                    we should provide good defaults, just as for script
            //                    environment builders.
            // TODO(Matthew): Do we want to provide pre & post install/update/uninstall 
            //                events for other mods to hook into?
            // TODO(Matthew): For scripted mods, do we want to let them themselves provide
            //                pre/post handlers for these procedures?
            class Installer {
                friend InstallStrategy;

                enum class EntryPointKind {
                    SINGLE,
                    MULTI
                };

                using InstallStrategies = std::vector<InstallStrategy*>;
                using EntryPoints       = std::unordered_map<nString, EntryPointKind>;
                using EntryData         = std::unordered_map<nString, std::vector<nString>>;
            public:
                Installer();
                ~Installer() {
                    // Empty.
                }

                /*!
                 * \brief Initialises the installer with necessary utilities.
                 *
                 * \param iom: The IO manager to be used for handling file access.
                 * \param installDir: The directory into which the mod file contents are placed for any necessary future reference (e.g. if mod load order gets changed).
                 * \param updateDir: The directory into which the mod file contents are placed for an update.
                 * \param globalModDir: The directory into which active mod files are installed.
                 */
                void init(vio::IOManager* iom, const nString& installDir, const nString& updateDir, const nString& globalModDir);
                /*!
                 * \brief Disposes of the installer.
                 */
                void dispose();

                /*!
                 * \brief Registers the given install strategy to the installer.
                 *
                 * \param strategy: The strategy to register.
                 *
                 * \return True if the strategy was successfully registered, false otherwise.
                 */
                bool registerInstallStrategy(InstallStrategy* strategy);

                /*!
                 * \brief Preloads the mod at the given filepath.
                 *
                 * This entails moving its contents into the appropriate
                 * sub-directory of installDir.
                 *
                 * \param filepath: The (absolute) filepath to the current mod location.
                 * \param forUpdate: If true, the mod is loaded into a temporary dir ready
                 * for update process.
                 * \param force: If true, any contents at the specified location will be
                 * deleted and the new contents preloaded.
                 *
                 * \return True if the mod was preloaded, false otherwise.
                 */
                bool preload(const vio::Path& filepath, bool forUpdate = false, bool force = false);

                /*!
                 * Installs the named mod.
                 *
                 * This entails merging edits into appropriate yaml files, installing
                 * assets into own mod directory, generating any extra files via scripted
                 * installer, etc..
                 *
                 * \param modName: The name of the mod to run installation for.
                 */
                bool install(const nString& modName);
                /*!
                 * Updates the named mod.
                 *
                 * This entails merging edits into appropriate yaml files, installing
                 * any new assets into own mod directory, generating any extra files via 
                 * scripted updater, etc..
                 *
                 * \param modName: The name of the mod to run update for.
                 */
                bool update(const nString& modName);
                /*!
                 * Uninstalls the named mod.
                 *
                 * This entails removing edits from appropriate yaml files, clearing own 
                 * mod directory, and running any uninstall procedure in scripted 
                 * uninstaller, etc..
                 *
                 * \param modName: The name of the mod to run update for.
                 */
                bool uninstall(const nString& modName);
            protected:
                /*!
                 * \brief Registers an entry point to be extracted from entry points file.
                 *
                 * In this case, the entry point names only one file to be accessed.
                 *
                 * \param entryPoint: Name of the entry point to register.
                 */
                void registerSingleEntryPoint(const nString& entryPoint);
                /*!
                 * \brief Registers an entry point to be extracted from entry points file.
                 *
                 * In this case, the entry point names multiple files to be accessed.
                 *
                 * \param entryPoint: Name of the entry point to register.
                 */
                void registerMultiEntryPoint(const nString& entryPoint);

                vio::IOManager* m_iomanager; ///< The IO manager used for file handling.

                nString m_installDir;   ///< The install directory that full mod contents are put into for future reference.
                nString m_updateDir;    ///< The update directory that full mod contents are put into for an update.
                nString m_globalModDir; ///< The global mod directory into which active mod contents are placed.

                InstallStrategies m_strategies;  ///< The list of strategies to be executed on various installer actions.
                EntryPoints       m_entryPoints; ///< A map of entry points to look for in the entry points file.
                EntryData         m_entryData;   ///< This is populated just prior to any given mod being actioned by
                                                 ///  this installer (be that install, update or uninstall). It is
                                                 ///  cleared after.
            };
        }
    }
}
namespace vmod = vorb::mod;

#endif // !Vorb_Installer_h__