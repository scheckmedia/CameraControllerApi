//
//  Command.h
//  CameraControllerApi
//
//  Created by Tobias Scheck on 09.08.13.
//  Copyright (c) 2013 scheck-media. All rights reserved.
//

#ifndef __CameraControllerApi__Command__
#define __CameraControllerApi__Command__

#include "Api.h"
#include <iostream>
#include <map>
#include <string>
#include <set>
#include <vector>

#define CCA_CMD_INVALID -1;
#define CCA_CMD_SUCCESS 1;

using std::map;
using std::string;
using std::set;
using std::vector;

namespace CameraControllerApi {
    class Command {
    public:
        Command(Api *api);
        int execute(const string& url, const map<string, string>& argvals, string& response);
    private:
        Api *_api;
        map<string, set<string> > _valid_commands;
        bool _executeAPI(const string &url, string action, const map<string, string> &urlparams, CCA_API_OUTPUT_TYPE type, string &response);
        bool _validate(const void *data);
    };
}

#endif /* defined(__CameraControllerApi__Command__) */
