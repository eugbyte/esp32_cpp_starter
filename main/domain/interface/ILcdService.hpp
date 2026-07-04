//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_EXAMPLE_ILCDSERVICE_H
#define CPP_EXAMPLE_ILCDSERVICE_H
#include <esp_err.h>
#include <string_view>

class ILcdService {
public:
	virtual ~ILcdService() = default;
	virtual esp_err_t SendText(std::string_view text) = 0;
	virtual esp_err_t Clear() = 0;
};

#endif // CPP_EXAMPLE_ILCDSERVICE_H
