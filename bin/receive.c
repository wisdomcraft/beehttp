
extern unsigned int debug;


int receiveHandle(char *receive, char *client_address){
    char *body          = strstr(receive, "\r\n\r\n");
    if(body == NULL){
        printf("NULL!!!!!!\r\n");
        return -1;
    }
    
    char *body_decode   = urldecode(body);

    char *data          = strstr(body_decode, "{");
    if(data == NULL)    return -1;
    if(debug == 1) printf("\r\npost data:\r\n%s\r\n----------------------------------------\r\n", data);
    
    cJSON *json         = cJSON_Parse(data);
    if(json == NULL){
        if(debug == 1) printf("error, post data is not json, receive.c #20, data content: %s\r\n", data);
        mylog("error", "post data is not json, receive.c #20\r\n");
        cJSON_Delete(json);
        return -1;
    }
    
    cJSON *api_node     = cJSON_GetObjectItem(json, "api");
    if(api_node == NULL){
        if(debug == 1) printf("error, there is not exists node 'api' in json, receive.c #28, data content: %s\r\n", data);
        mylog("error", "there is not exists node 'api' in json, receive.c #28\r\n");
        cJSON_Delete(json);
        return -1;
    }
    if(api_node->type != cJSON_String){
        if(debug == 1) printf("error, json node 'api' type is not string, receive.c #34, data content: %s\r\n", data);
        mylog("error", "json node 'api' type is not string, receive.c #34\r\n");
        cJSON_Delete(json);
        return -1;
    }
    if(strcmp(api_node->valuestring, "host_information") != 0){
        if(debug == 1) printf("error, json node 'api' value is not host_information, receive.c #40, data content: %s\r\n", data);
        mylog("error", "json node 'api' value is not host_information, receive.c #40\r\n");
        cJSON_Delete(json);
        return -1;
    }
    
    cJSON *sn_node      = cJSON_GetObjectItem(json, "serial_no");
    if(sn_node == NULL){
        if(debug == 1) printf("error, there is not exists node 'serial_no(sn)' in json, receive.c #48, data content: %s\r\n", data);
        mylog("error", "there is not exists node 'serial_no(sn)' in json, receive.c #48\r\n");
        cJSON_Delete(json);
        return -1;
    }
    if(sn_node->type != cJSON_String){
        if(debug == 1) printf("error, json node 'sn' type is not string, receive.c #54, data content: %s\r\n", data);
        mylog("error", "json node 'sn' type is not string, receive.c #54\r\n");
        cJSON_Delete(json);
        return -1;
    }
    char *sn = sn_node->valuestring;
    if(strlen(sn)<20){
        if(debug == 1) printf("error, sn is short than 20, receive.c #61, data content: %s\r\n", data);
        mylog("error", "sn is short than 20, receive.c #61\r\n");
        cJSON_Delete(json);
        return -1;
    }
    if(strlen(sn)>30){
        if(debug == 1) printf("error, sn it long than 30, receive.c #67, data content: %s\r\n", data);
        mylog("error", "sn it long than 30, receive.c #67\r\n");
        cJSON_Delete(json);
        return -1;
    }
    
    cJSON *update       = cJSON_CreateObject();
    cJSON_AddItemToObject(update, "$setOnInsert", cJSON_CreateRaw("{\"groupid\":0}"));
    
    cJSON *insert       = cJSON_CreateObject();
    cJSON_AddItemToObject(insert, "sn", cJSON_CreateString(sn));

    cJSON *set          = cJSON_CreateObject();
    cJSON_AddItemToObject(set, "sn", cJSON_CreateString(sn));
    
    cJSON *alias_node   = cJSON_GetObjectItem(json, "alias");
    if(alias_node != NULL){
        if(alias_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_alias",  cJSON_CreateString(alias_node->valuestring));
            cJSON_AddItemToObject(insert,   "alias",        cJSON_CreateString(alias_node->valuestring));
        }
    }
    
    cJSON *ip_node      = cJSON_GetObjectItem(json, "ip_addr");
    if(ip_node != NULL){
        if(ip_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_ip",     cJSON_CreateString(ip_node->valuestring));
            cJSON_AddItemToObject(insert,   "ip",           cJSON_CreateString(ip_node->valuestring));
        }
    }
    
    cJSON_AddItemToObject(set,      "state_out_ip",         cJSON_CreateString(client_address));
    cJSON_AddItemToObject(insert,   "out_ip",               cJSON_CreateString(client_address));
    
    cJSON *keepalive_time_node  = cJSON_GetObjectItem(json, "keepalive_time");
    if(keepalive_time_node != NULL){
        if(keepalive_time_node->type == cJSON_Number){
            char keepalive_time_string[11];
            sprintf(keepalive_time_string, "%d", keepalive_time_node->valueint);
            cJSON_AddItemToObject(set,      "state_keepalive_time", cJSON_CreateString(keepalive_time_string));
            cJSON_AddItemToObject(insert,   "keepalive_time",       cJSON_CreateString(keepalive_time_string));
        }else if(keepalive_time_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_keepalive_time", cJSON_CreateString(keepalive_time_node->valuestring));
            cJSON_AddItemToObject(insert,   "keepalive_time",       cJSON_CreateString(keepalive_time_node->valuestring));
        }
    }
    
    cJSON *device_factory_node  = cJSON_GetObjectItem(json, "device_factory");
    if(device_factory_node != NULL){
        if(device_factory_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_device_factory", cJSON_CreateString(device_factory_node->valuestring));
            cJSON_AddItemToObject(insert,   "device_factory",       cJSON_CreateString(device_factory_node->valuestring));
        }
    }
    
    cJSON *model_node   = cJSON_GetObjectItem(json, "model");
    if(model_node != NULL){
        if(model_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_model",  cJSON_CreateString(model_node->valuestring));
            cJSON_AddItemToObject(insert,   "model",        cJSON_CreateString(model_node->valuestring));
        }
    }
    
    cJSON *cpu_node     = cJSON_GetObjectItem(json, "cpu");
    if(cpu_node != NULL){
        if(cpu_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_cpu",    cJSON_CreateString(cpu_node->valuestring));
            cJSON_AddItemToObject(insert,   "cpu",          cJSON_CreateString(cpu_node->valuestring));
        }
    }
    
    cJSON *memory_node  = cJSON_GetObjectItem(json, "memory");
    if(memory_node != NULL){
        if(memory_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_memory", cJSON_CreateString(memory_node->valuestring));
            cJSON_AddItemToObject(insert,   "memory",       cJSON_CreateString(memory_node->valuestring));
        }
    }
    
    cJSON *disk_node  = cJSON_GetObjectItem(json, "disk");
    if(disk_node != NULL){
        if(disk_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_disk",   cJSON_CreateString(disk_node->valuestring));
            cJSON_AddItemToObject(insert,   "disk",         cJSON_CreateString(disk_node->valuestring));
        }
    }
    
    cJSON *user_count_node  = cJSON_GetObjectItem(json, "user_count");
    if(user_count_node != NULL){
        if(user_count_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_user_count", cJSON_CreateString(user_count_node->valuestring));
            cJSON_AddItemToObject(insert,   "user_count",       cJSON_CreateString(user_count_node->valuestring));
        }
    }
    
    cJSON *bytes_in_node  = cJSON_GetObjectItem(json, "bytes_in");
    if(bytes_in_node != NULL){
        if(bytes_in_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_bytes_in",   cJSON_CreateString(bytes_in_node->valuestring));
            cJSON_AddItemToObject(insert,   "bytes_in",         cJSON_CreateString(bytes_in_node->valuestring));
        }
    }
    
    cJSON *bytes_out_node  = cJSON_GetObjectItem(json, "bytes_out");
    if(bytes_out_node != NULL){
        if(bytes_out_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_bytes_out",  cJSON_CreateString(bytes_out_node->valuestring));
            cJSON_AddItemToObject(insert,   "bytes_out",        cJSON_CreateString(bytes_out_node->valuestring));
        }
    }
    
    cJSON *sys_time_node  = cJSON_GetObjectItem(json, "sys_time");
    if(sys_time_node != NULL){
        if(sys_time_node->type == cJSON_Number){
            char sys_time_string[11];
            sprintf(sys_time_string, "%d", sys_time_node->valueint);
            cJSON_AddItemToObject(set,      "state_device_time",    cJSON_CreateString(sys_time_string));
            cJSON_AddItemToObject(insert,   "device_time",          cJSON_CreateString(sys_time_string));
        }else if(sys_time_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_device_time",    cJSON_CreateString(sys_time_node->valuestring));
            cJSON_AddItemToObject(insert,   "device_time",          cJSON_CreateString(sys_time_node->valuestring));
        }
    }
    
    time_t now;
    int unix_time       = time(&now);
    char unix_time_string[11];
    sprintf(unix_time_string, "%d", unix_time);
    cJSON_AddItemToObject(set,      "state_getinfo_time",           cJSON_CreateString(unix_time_string));
    cJSON_AddItemToObject(insert,   "getinfo_time",                 cJSON_CreateString(unix_time_string));
    
    if(keepalive_time_node != NULL){
        if(keepalive_time_node->type == cJSON_Number){
            int offline_time        = unix_time + (keepalive_time_node->valueint)*3;
            cJSON_AddItemToObject(set, "state_offline_time", cJSON_CreateNumber(offline_time));
        }else if(keepalive_time_node->type == cJSON_String){
            int keepalive_time_int  = atoi(keepalive_time_node->valuestring);
            int offline_time        = unix_time + (keepalive_time_int)*3;
            cJSON_AddItemToObject(set, "state_offline_time", cJSON_CreateNumber(offline_time));
        }
    }
    
    cJSON * place_num_node  = cJSON_GetObjectItem(json, "place_num");
    if(place_num_node != NULL){
        if(place_num_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_place_num", cJSON_CreateString(place_num_node->valuestring));
            cJSON_AddItemToObject(insert,   "place_num",       cJSON_CreateString(place_num_node->valuestring));
        }
    }
    
    cJSON * ap_num_node     = cJSON_GetObjectItem(json, "ap_num");
    if(ap_num_node != NULL){
        if(ap_num_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_ap_num", cJSON_CreateString(ap_num_node->valuestring));
            cJSON_AddItemToObject(insert,   "ap_num",       cJSON_CreateString(ap_num_node->valuestring));
        }
    }
    
    cJSON * all_log_num_node        = cJSON_GetObjectItem(json, "all_log_num");
    if(all_log_num_node != NULL){
        if(all_log_num_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_all_log_num", cJSON_CreateString(all_log_num_node->valuestring));
            cJSON_AddItemToObject(insert,   "all_log_num",       cJSON_CreateString(all_log_num_node->valuestring));
        }
    }
    
    cJSON * terminal_log_num_node   = cJSON_GetObjectItem(json, "terminal_log_num");
    if(terminal_log_num_node != NULL){
        if(terminal_log_num_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_terminal_log_num", cJSON_CreateString(terminal_log_num_node->valuestring));
            cJSON_AddItemToObject(insert,   "terminal_log_num",       cJSON_CreateString(terminal_log_num_node->valuestring));
        }
    }
    
    cJSON * vid_log_num_node        = cJSON_GetObjectItem(json, "vid_log_num");
    if(vid_log_num_node != NULL){
        if(vid_log_num_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_vid_log_num", cJSON_CreateString(vid_log_num_node->valuestring));
            cJSON_AddItemToObject(insert,   "vid_log_num",       cJSON_CreateString(vid_log_num_node->valuestring));
        }
    }
    
    cJSON * net_log_num_node        = cJSON_GetObjectItem(json, "net_log_num");
    if(net_log_num_node != NULL){
        if(net_log_num_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_net_log_num", cJSON_CreateString(net_log_num_node->valuestring));
            cJSON_AddItemToObject(insert,   "net_log_num",       cJSON_CreateString(net_log_num_node->valuestring));
        }
    }
    
    cJSON * other_log_num_node      = cJSON_GetObjectItem(json, "other_log_num");
    if(other_log_num_node != NULL){
        if(other_log_num_node->type == cJSON_String){
            cJSON_AddItemToObject(set,      "state_other_log_num", cJSON_CreateString(other_log_num_node->valuestring));
            cJSON_AddItemToObject(insert,   "other_log_num",       cJSON_CreateString(other_log_num_node->valuestring));
        }
    }
    
    cJSON_AddItemToObject(update, "$set", cJSON_CreateRaw(cJSON_PrintUnformatted(set)));

    char *update_json   = cJSON_PrintUnformatted(update);
    
    char *insert_json   = cJSON_PrintUnformatted(insert);
    
    cacheToMongodb(sn, update_json, insert_json);
    
    cJSON_Delete(set);
    cJSON_Delete(update);
    cJSON_Delete(json);
    
    return 0;
}


