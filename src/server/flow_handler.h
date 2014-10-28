/*
 * flow_handler.h
 *
 *  Created on: Oct 28, 2014
 *      Author: liao
 */

#ifndef FLOW_HANDLER_H_
#define FLOW_HANDLER_H_

class FlowHandler {

public:
	virtual int do_handle(char *flow_bytes, int size) {
		return 0;
	};
};

#endif /* FLOW_HANDLER_H_ */
