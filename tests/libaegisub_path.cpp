// Copyright (c) 2010, Amar Takhar <verm@aegisub.org>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
// $Id$

/// @file libaegisub_path.cpp
/// @brief agi::Path (Stored paths)
/// @ingroup mru

#include <libaegisub/path.h>
#include "main.h"
#include "util.h"

class lagi_path : public libagi {
protected:
	std::string default_path;
	std::string conf_ok;

	virtual void SetUp() {
		default_path = "{\"Config\" : \"/a/real/path\"}";
		conf_ok = "./data/path_ok.json";
	}
};

class PathTest : public agi::Path {
public:
	PathTest(const std::string &file, const std::string& default_path) :
	Path(file, default_path),
	data(Data()),
	config(Config()),
	user(User()),
	temp(Temp())
	{}

	const std::string data;
	const std::string config;
	const std::string user;
	const std::string temp;
};

TEST_F(lagi_path, ConstructFromFile) {
	EXPECT_NO_THROW(agi::Path path(conf_ok, default_path));
}

TEST_F(lagi_path, CheckCookieData) {
	PathTest path("data/path_cookie.json", default_path);
	ASSERT_EQ(path.Get("Data"), path.data);
}

TEST_F(lagi_path, CheckCookieConfig) {
	PathTest path("data/path_cookie.json", default_path);
	ASSERT_EQ(path.Get("Config"), path.config);
}

TEST_F(lagi_path, CheckCookieUser) {
	PathTest path("data/path_cookie.json", default_path);
	ASSERT_EQ(path.Get("User"), path.user);
}

TEST_F(lagi_path, CheckCookieTemp) {
	PathTest path("data/path_cookie.json", default_path);
	ASSERT_EQ(path.Get("Temp"), path.temp);
}
