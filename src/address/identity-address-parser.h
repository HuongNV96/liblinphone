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

#ifndef _L_IDENTITY_ADDRESS_PARSER_H_
#define _L_IDENTITY_ADDRESS_PARSER_H_

#include "identity-address.h"
#include "object/singleton.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class IdentityAddressParserPrivate;

class IdentityAddressParser : public Singleton<IdentityAddressParser> {
	friend class Singleton<IdentityAddressParser>;

public:
	std::shared_ptr<IdentityAddress> parseAddress (const std::string &input);

private:
	IdentityAddressParser ();

	L_DECLARE_PRIVATE(IdentityAddressParser);
	L_DISABLE_COPY(IdentityAddressParser);
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_IDENTITY_ADDRESS_PARSER_H_
