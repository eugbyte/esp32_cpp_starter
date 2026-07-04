//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_EXAMPLE_ILCDSERVICE_H
#define CPP_EXAMPLE_ILCDSERVICE_H
#include "etl/string_view.h"
#include <esp_err.h>

class ILcdService {
public:
	virtual ~ILcdService() = default;
	virtual esp_err_t SendText(etl::string_view text) = 0;
	virtual esp_err_t Clear() = 0;
};

#endif // CPP_EXAMPLE_ILCDSERVICE_H
