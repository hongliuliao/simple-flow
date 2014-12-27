/*
 * stat_http_handler.h
 *
 *  Created on: Dec 27, 2014
 *      Author: liao
 */

#ifndef STAT_HTTP_HANDLER_H_
#define STAT_HTTP_HANDLER_H_

#include <vector>
#include <fstream>
#include <sstream>
#include "json/json.h"
#include "hiredis/hiredis.h"
#include "simple_log.h"
#include "http_server.h"

Response get_statist_info(Request& request);

Response static_source_handler(Request& request);

#endif /* STAT_HTTP_HANDLER_H_ */
