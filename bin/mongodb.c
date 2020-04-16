
extern unsigned int debug;


char *collectionName(void){
    time_t now;
    struct tm *p;
    time(&now);
    p = localtime(&now);
    
    char year[8];
    sprintf(year, "%d", (1900+p->tm_year));
    char month[2];
    if((p->tm_mon) < 9){
        sprintf(month, "0%d", (1+p->tm_mon));
    }else{
        sprintf(month, "%d", (1+p->tm_mon));
    }
    char day[2];
    if((p->tm_mday) < 10){
        sprintf(day, "0%d", (p->tm_mday));
    }else{
        sprintf(day, "%d", (p->tm_mday));
    }
    
    strcat(year, month);
    strcat(year, day);
    
    char *collection = (char*)malloc(1);
    strcpy(collection, "");
    if(realloc(collection, strlen(year)+8) == NULL){
        if(debug == 1) printf("error, realloc() failed in log.c #32");
        exit(0);
    }
    strcat(collection, "abt_sn_");
    strcat(collection, year);
    
    return collection;
}


int cacheToMongodb(char *sn, char *update_json, char *insert_json){
    mongoc_client_t     *client;
    mongoc_database_t   *database;
    mongoc_collection_t *collection, *collection2;
    bson_error_t        error;
    
    mongoc_init();
    client      = mongoc_client_new("mongodb://192.168.1.111:27017");
    
    database    = mongoc_client_get_database(client, "cloud");
    collection  = mongoc_client_get_collection(client, "cloud", "abt_device");
    char *collection_name = collectionName();
    collection2 = mongoc_client_get_collection(client, "cloud", collection_name);
    
    bson_t *update_query    = BCON_NEW("_id", BCON_UTF8(sn));
    bson_t *update_document = bson_new_from_json((const uint8_t *)update_json, -1, &error);
    if(!update_document){
        if(debug == 1) printf("error, update document created failed by bson_new_from_json, mongodb.c 58#, %s\r\n", error.message);
        mylog("error", "update document created failed by bson_new_from_json, mongodb.c 58#\r\n");
        goto out;
    }
    
    if(!mongoc_collection_update(collection, MONGOC_UPDATE_UPSERT, update_query, update_document, NULL, &error)){
        if(debug == 1) printf("error, update failed in mongodb.c 64#, %s\r\n", error.message);
        mylog("error", "update failed in mongodb.c 64#\r\n");
        goto out;
    }
    
    bson_t *insert_document = bson_new_from_json((const uint8_t *)insert_json, -1, &error);
    if(!insert_document){
        if(debug == 1) printf("error, insert document created failed by bson_new_from_json, mongodb.c 71#, %s\r\n", error.message);
        mylog("error", "insert document created failed by bson_new_from_json, mongodb.c 71#\r\n");
        bson_destroy(insert_document);
        goto out;
    }
    if(!mongoc_collection_insert(collection2, MONGOC_INSERT_NONE, insert_document, NULL, &error)){
        if(debug == 1) printf("error, insert failed in mongodb.c 77#, %s\r\n", error.message);
        mylog("error", "insert failed in mongodb.c 77#\r\n");
        bson_destroy(insert_document);
        goto out;
    }

out:
    bson_destroy(update_query);
    bson_destroy(update_document);
    
    mongoc_collection_destroy(collection);
    mongoc_database_destroy(database);
    mongoc_client_destroy(client);
    mongoc_cleanup();
    
    return 0;
}




