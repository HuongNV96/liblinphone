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

#ifndef LINPHONE_LDAP_CONTACT_SEARCH_H_
#define LINPHONE_LDAP_CONTACT_SEARCH_H_

#include "linphone/core.h"
#include "linphone/types.h"
#include "linphone/contactprovider.h"
#include "belle-sip/object++.hh"
#include "core/core.h"
#include "core/core-accessor.h"
#include "../search/search-result.h"
#include <map>
#include <vector>
#include <string>
#include <list>

#include <ldap.h>

LINPHONE_BEGIN_NAMESPACE

class LdapContactProvider;

class LINPHONE_PUBLIC LdapContactSearch {
public:
	LdapContactSearch(const int& msgId);
	LdapContactSearch(LdapContactProvider * parent, std::string predicate, ContactSearchCallback cb, void* cbData);
	virtual ~LdapContactSearch();
	
	void callCallback();
	
	static int entryCompareWeak(const void*a, const void* b);
	
	int mMsgId;
	std::string mFilter;
	bool_t  complete;
	bool_t mHaveMoreResults = FALSE;
	std::list<std::shared_ptr<SearchResult>> mFoundEntries;
	unsigned int mFoundCount;
	
private:
	std::string mPredicate;
	ContactSearchCallback mCb;
	void* mCbData;
};

LINPHONE_END_NAMESPACE

#endif /* LINPHONE_LDAP_CONTACT_SEARCH_H_ */
