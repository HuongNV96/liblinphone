
#[[
Copyright (c) 2010-2022 Belledonne Communications SARL.

This file is part of Liblinphone 
(see https://gitlab.linphone.org/BC/public/liblinphone).

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
]]

list(APPEND LINPHONE_PRIVATE_HEADER_FILES
	bellesip_sal/sal_impl.h
	carddav.h
	conference_private.h
	contact_providers_priv.h
	core_private.h
	enum.h
	lime.h
	lpc2xml.h
	private.h
	quality_reporting.h
	sqlite3_bctbx_vfs.h
	vcard_private.h
	xml2lpc.h
)

set(LINPHONE_SOURCE_FILES_C
	authentication.c
	bellesip_sal/sal_address_impl.c
	bellesip_sal/sal_impl.c
	bellesip_sal/sal_op_impl.c
	bellesip_sal/sal_sdp.c
	buffer.c
	callbacks.c
	carddav.c
	chat.c
	contactprovider.c
	dict.c
	ec-calibrator.c
	echo-tester.c
	enum.c
	error_info.c
	event.c
	friend.c
	friendlist.c
	im_notif_policy.c
	info.c
	lime.c
	im_encryption_engine.c
	linphonecall.c
	linphonecore.c
	linphoneconference.c
	linphone_tunnel_config.c
	localplayer.c
	logging.c
	lpc2xml.c
	lpconfig.c
	lsd.c
	misc.c
	payload_type.c
	player.c
	presence.c
	proxy.c
	quality_reporting.c
	remote_provisioning.c
	ringtoneplayer.c
	siplogin.c
	sipsetup.c
	sqlite3_bctbx_vfs.c
	update_check.c
	video_definition.c
	xml2lpc.c
	xml.c
	xmlrpc.c
	vtables.c
)
set(LINPHONE_SOURCE_FILES_CXX
	conference.cc
	tester_utils.cpp
)

if(ENABLE_JAVA_WRAPPER)
	list(APPEND LINPHONE_SOURCE_FILES_CXX ${LINPHONE_JNI_SOURCES})
	set_source_files_properties(${LINPHONE_JNI_SOURCES} PROPERTIES GENERATED TRUE)
endif()

set(LINPHONE_SOURCE_FILES_OBJC)
if (IOS)
	list(APPEND LINPHONE_SOURCE_FILES_OBJC ringtoneplayer_ios.m ringtoneplayer_ios.h)
	list(APPEND LINPHONE_SOURCE_FILES_CXX linphonecore_ios.cc)
endif()


if(ENABLE_TUNNEL)
	list(APPEND LINPHONE_SOURCE_FILES_CXX
		linphone_tunnel.cc
		TunnelManager.cc
	)
	add_definitions(-DTUNNEL_ENABLED)
else()
	list(APPEND LINPHONE_SOURCE_FILES_C linphone_tunnel_stubs.c)
endif()

if(BELCARD_FOUND)
	list(APPEND LINPHONE_SOURCE_FILES_CXX vcard.cc)
else()
	list(APPEND LINPHONE_SOURCE_FILES_C vcard_stubs.c)
endif()


bc_git_version(liblinphone ${PROJECT_VERSION})

add_definitions(
	-DUSE_BELLESIP
	-DBCTBX_LOG_DOMAIN="liblinphone"
	-DLINPHONE_EXPORTS
)

set_source_files_properties(${LINPHONE_SOURCE_FILES_C} PROPERTIES LANGUAGE CXX)
bc_apply_compile_flags(LINPHONE_SOURCE_FILES_C STRICT_OPTIONS_CPP STRICT_OPTIONS_CXX)
bc_apply_compile_flags(LINPHONE_SOURCE_FILES_CXX STRICT_OPTIONS_CPP STRICT_OPTIONS_CXX)
bc_apply_compile_flags(LINPHONE_SOURCE_FILES_OBJC STRICT_OPTIONS_CPP STRICT_OPTIONS_OBJC)


if (ENABLE_STATIC)
	add_library(linphone-coreapi-static OBJECT
		${LINPHONE_PRIVATE_HEADER_FILES} ${LINPHONE_SOURCE_FILES_C} ${LINPHONE_SOURCE_FILES_CXX} ${LINPHONE_SOURCE_FILES_OBJC}
	)
	target_include_directories(linphone-coreapi-static PRIVATE ${LINPHONE_INCLUDE_DIRS})
	add_dependencies(linphone-coreapi-static liblinphone-git-version)
	if (ENABLE_JAVA_WRAPPER)
		add_dependencies(linphone-coreapi-static linphonej)
	endif()
endif ()

if (ENABLE_SHARED)
	add_library(linphone-coreapi OBJECT
		${LINPHONE_PRIVATE_HEADER_FILES} ${LINPHONE_SOURCE_FILES_C} ${LINPHONE_SOURCE_FILES_CXX} ${LINPHONE_SOURCE_FILES_OBJC}
	)
	target_compile_options(linphone-coreapi PRIVATE "-fPIC")
	target_include_directories(linphone-coreapi PRIVATE
		${LINPHONE_INCLUDE_DIRS}
		$<TARGET_PROPERTY:${BELLESIP_TARGETNAME},INTERFACE_INCLUDE_DIRECTORIES>
		$<TARGET_PROPERTY:belr,INTERFACE_INCLUDE_DIRECTORIES>
		$<TARGET_PROPERTY:mediastreamer,INTERFACE_INCLUDE_DIRECTORIES>
		$<TARGET_PROPERTY:ortp,INTERFACE_INCLUDE_DIRECTORIES>
	)
	add_dependencies(linphone-coreapi liblinphone-git-version)
	if (ENABLE_JAVA_WRAPPER)
        add_dependencies(linphone-coreapi linphonej)
	endif()
endif ()


add_subdirectory(help)
