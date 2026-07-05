//
// Created by eugen on 7/4/2026.
//

#ifndef CPP_EXAMPLE_ILCDSERVICE_H
#define CPP_EXAMPLE_ILCDSERVICE_H
#include <esp_err.h>
#include <etl/string_view.h>

class ILcdService {
public:
	virtual ~ILcdService() = default;
	virtual esp_err_t send_text(etl::string_view text) = 0;
	virtual esp_err_t clear() = 0;
};

#endif // CPP_EXAMPLE_ILCDSERVICE_H
