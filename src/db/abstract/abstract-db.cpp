/*
 * Copyright (c) 2010-2022 Belledonne Communications SARL.
 *
 * This file is part of Liblinphone 
 * (see https://gitlab.linphone.org/BC/public/liblinphone).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef __APPLE__
	#include <TargetConditionals.h>
#endif // ifdef __APPLE__

#if (__APPLE__ || defined(__ANDROID__))
	#include <sqlite3.h>
#endif // if (__APPLE__ || defined(__ANDROID__))

#include "abstract-db-p.h"
#include "logger/logger.h"

// =============================================================================

using namespace std;

LINPHONE_BEGIN_NAMESPACE

#if (__APPLE__ || defined(__ANDROID__))
	// Force static sqlite3 linking for IOS and Android.
	extern "C" void register_factory_sqlite3();

#ifdef HAVE_DB_STORAGE
	static void sqlite3Log (void *, int iErrCode, const char *zMsg) {
		lInfo() << "[sqlite3][" << iErrCode << "]" << zMsg;
	}
#endif
#endif // if (TARGET_OS_IPHONE || defined(__ANDROID__))

void AbstractDbPrivate::safeInit () {
#ifdef HAVE_DB_STORAGE
	L_Q();
	dbSession.enableForeignKeys(false);
	q->init();
	dbSession.enableForeignKeys(true);
	initialized = true;
#endif
}

AbstractDb::AbstractDb (AbstractDbPrivate &p) : Object(p) {}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif // _MSC_VER
bool AbstractDb::connect (Backend backend, const string &nameParams) {
#ifdef HAVE_DB_STORAGE
	L_D();

	#if (__APPLE__ || defined(__ANDROID__))
		if (backend == Sqlite3) {
			static bool registered = false;
			if (!registered) {
				registered = true;
				register_factory_sqlite3();
				sqlite3_config(SQLITE_CONFIG_LOG, sqlite3Log, nullptr);
			}
		}
	#endif // if (TARGET_OS_IPHONE || defined(__ANDROID__))

	d->backend = backend;
	d->dbSession = DbSession(
		(backend == Mysql ? "mysql://" : "sqlite3://") + nameParams
	);

	if (d->dbSession) {
		try {
			d->safeInit();
		} catch (const exception &e) {
			lError() << "Unable to init database: " << e.what();

			// Reset session.
			d->dbSession = DbSession();
		}
	}

	return d->dbSession;
#else
	return false;
#endif
}
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif // _MSC_VER

void AbstractDb::disconnect () {
#ifdef HAVE_DB_STORAGE
	L_D();
	d->dbSession = DbSession();
#endif
}

bool AbstractDb::forceReconnect () {
#ifdef HAVE_DB_STORAGE
	L_D();
	if (!d->dbSession) {
		lWarning() << "Unable to reconnect. Not a valid database session.";
		return false;
	}

	constexpr int retryCount = 2;
	lInfo() << "Trying sql backend reconnect...";

	try {
		for (int i = 0; i < retryCount; ++i) {
			try {
				lInfo() << "Reconnect... Try: " << i;
				d->dbSession.getBackendSession()->reconnect(); // Equivalent to close and connect.
				d->safeInit();
				lInfo() << "Database reconnection successful!";
				return true;
			} catch (const soci::soci_error &e) {
				if (e.get_error_category() != soci::soci_error::connection_error)
				throw e;
			}
		}
	} catch (const exception &e) {
		lError() << "Unable to reconnect: `" << e.what() << "`.";
		return false;
	}

	lError() << "Database reconnection failed!";
#endif
	return false;
}

AbstractDb::Backend AbstractDb::getBackend () const {
	L_D();
	return d->backend;
}

bool AbstractDb::import (Backend, const string &) {
	return false;
}

// -----------------------------------------------------------------------------

void AbstractDb::init () {
	// Nothing.
}

bool AbstractDb::isInitialized() const{
	L_D();
	return d->initialized;
}

std::ostream& operator<<(std::ostream& os, AbstractDb::Backend b){
	switch(b){
		case AbstractDb::Mysql:
			os<<"Mysql";
		break;
		case AbstractDb::Sqlite3:
			os<<"Sqlite3";
		break;
	}
	return os;
}

LINPHONE_END_NAMESPACE
