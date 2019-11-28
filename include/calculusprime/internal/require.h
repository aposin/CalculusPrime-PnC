/*
Copyright 2019 Association for the promotion of open - source insurance software and for the establishment of open interface standards in the insurance industry

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/ 
#ifndef CP_REQUIRE_H
#define CP_REQUIRE_H 1

#include <sstream>
#include <stdexcept>

#define CP_REQUIRE(cond) if (!(cond)) { \
		std::ostringstream s__; \
    	s__ << "Assertion '" << #cond << "' in: " << __FILE__ << " line " << __LINE__ << " failed!"; \
		throw std::runtime_error(s__.str()); }


#endif // #ifndef CP_REQUIRE_H
