# RedisNoSQL
K/V - oriented DB

     [1 byte type] [key] [a type specific value]
     
# KV-riented Collection, 鍵值對導向存儲的資料集合

是分散式檔案系統的實現，相對於傳統 RDBMS 的行式 row，Redis 則以鍵值對的方式儲存資料，另外，還有 AWS 的 Dynamo DB 和 Redis 相同，一樣使用鍵值對方式對資料進行儲存。（其他 noSQL 還有列式和文件式，共三種資料庫。）
  
  https://github.com/QuinoaPy/Cassandra (Column-Oriented, 列式存儲的資料集合)
  
  https://github.com/QuinoaPy/MongoDB (Doc-Oriented, 文件式存儲的資料集合) 
  
Intro, 簡介

當使用者在使用後端所開發的網站或手機前端的應用程式時，會產生對應資料，例如：Ａuth 登入驗證、Cart 購物車，這些資料需要儲存在後端後方的 DB 管理，這就是資料管理系統。

相較於關聯式資料庫，儲存前要先定義每個欄位 cloumn (即 field) 的資料型態，之後便僅能在對應的欄位儲存對應型態的資料，而資料庫 DB 中擁有多個資料表 Schema ，資料表之間可以利用 id 建立關聯，並且透過資料表結構 schema 確認資料表中各種資料間的關聯。

然而近年來，網站系統訪問量攀升，產生大筆多筆的資料數據，許多資料庫因為讀寫頻率高，導致性能問題產生，關聯式資料庫也因為欄位要預先設定型別，但是當寫入的資料太龐大時，這對資料庫管理員來說會越來越複雜和面對壓力。

這時，非關聯式資料庫出場！它補足了關聯式資料庫的不足，增加擴增和彈性。

非關聯式則是利用 k/v 鍵值對格式或是列式 column 存儲資料集合 collection，取代傳統的資料表 table 和 schema。 (相對於關聯式資料庫是利用 Row 行式來建立資料表 Table 。)

# Less spcial Data types, 不佔空間的資料型別

Redis 2.2 uses less space up to a certain size. 

          Hashes（雜湊）, Lists（串列）, Sets（集合）, Zsets（有序集合）

Hashes, Lists, Sets composed of just integers, and Sorted Sets, when smaller than a given number of elements, and up to a maximum element size, are encoded in a very memory efficient way that uses up to 10 times less memory (with 5 time less memory used being the average saving).

This is completely transparent from the point of view of the user and API. Since this is a CPU / memory trade off it is possible to tune the maximum number of elements and maximum element size for special encoded types using the following redis.conf directives.

備註：串列比陣列具備的優勢是不必事先宣告定義記憶體的大小，然而因為串列使用額外記憶體記憶節點指標，故消耗資源，而且需要以迭代方式進行查詢，所以查詢時間比陣列速度還慢，不過串列是方便新刪修的，因為節點指標的關係。

# AOF rewrite, 只允許追加不允許改寫檔案

   Append Only File

     void RedisModule_EmitAOF(
     RedisModuleIO *io, const char *cmdname, const char *fmt, ...
     );

# use Hash, 使用雜湊函式

          require 'rubygems'
          require 'redis'

          UseOptimization = true

          def hash_get_key_field(key)
              s = key.split(":")
              if s[1].length > 2
                  {:key => s[0]+":"+s[1][0..-3], :field => s[1][-2..-1]}
              else
                  {:key => s[0]+":", :field => s[1]}
              end
          end

          def hash_set(r,key,value)
              kf = hash_get_key_field(key)
              r.hset(kf[:key],kf[:field],value)
          end

          def hash_get(r,key,value)
              kf = hash_get_key_field(key)
              r.hget(kf[:key],kf[:field],value)
          end

          r = Redis.new
          (0..100000).each{|id|
              key = "object:#{id}"
              if UseOptimization
                  hash_set(r,key,"val")
              else
                  r.set(key,"val")
              end
          }
          
# register New Data Type, 註冊新的資料型別

          static RedisModuleType *MyType;
          #define MYTYPE_ENCODING_VERSION 0

          int RedisModule_OnLoad(RedisModuleCtx *ctx) {
          
               RedisModuleTypeMethods tm = {
               
                   .version = REDISMODULE_TYPE_METHOD_VERSION,
                   .rdb_load = MyTypeRDBLoad,
                   .rdb_save = MyTypeRDBSave,
                   .aof_rewrite = MyTypeAOFRewrite,
                   .free = MyTypeFree
                   
               };

               MyType = RedisModule_CreateDataType(ctx, "MyType-AZ",
    MYTYPE_ENCODING_VERSION, &tm);
               
               if (MyType == NULL) 
               return REDISMODULE_ERR;
                   
          }
          
# Keys Getter & Setter

     RedisModuleKey *key = RedisModule_OpenKey(ctx,keyname,REDISMODULE_WRITE);

     struct some_private_struct *data = createMyDataStructure();

     RedisModule_ModuleTypeSetValue(key,MyType,data);
     
test>

          if (RedisModule_ModuleTypeGetType(key) == MyType) {
          
              /* ... do something ... */
              
          }
  
test>

     RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
         REDISMODULE_READ|REDISMODULE_WRITE);
         
     int type = RedisModule_KeyType(key);
     
     if (type != REDISMODULE_KEYTYPE_EMPTY &&
         RedisModule_ModuleTypeGetType(key) != MyType) {
         
         return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
         
     }
     
write data to the data type>

     /* Create an empty value object if the key is currently empty. */
     
     struct some_private_struct *data;
     
     if (type == REDISMODULE_KEYTYPE_EMPTY) {
     
         data = createMyDataStructure();
         RedisModule_ModuleTypeSetValue(key,MyTyke,data);
         
     } else {
     
         data = RedisModule_ModuleTypeGetValue(key);
         
     }
     /* Do something with 'data'... */
     
# method called to Free memory, 釋放記憶體的方法呼叫

     typedef void (*RedisModuleTypeFreeFunc)(void *value);
     
     void MyTypeFreeCallback(void *value) {
     
          RedisModule_Free(value);
          
     }
     
# method called to Save & Load, 存取的方法呼叫

     struct double_array {
         size_t count;
         double *values;
     };
     
     void DoubleArrayRDBSave(RedisModuleIO *io, void *ptr) {
     
         struct dobule_array *da = ptr;
         
         RedisModule_SaveUnsigned(io,da->count);
         
         for (size_t j = 0; j < da->count; j++)
             RedisModule_SaveDouble(io,da->values[j]);
        
     }
     
     
     void *DoubleArrayRDBLoad(RedisModuleIO *io, int encver) {
     
         if (encver != DOUBLE_ARRAY_ENC_VER) {
         
             /* We should actually log an error here, or try to implement
                the ability to load older versions of our data structure. */
             return NULL;
             
         }

         struct double_array *da;
         da = RedisModule_Alloc(sizeof(*da));
         da->count = RedisModule_LoadUnsigned(io);
         da->values = RedisModule_Alloc(da->count * sizeof(double));
         
         for (size_t j = 0; j < da->count; j++)
             da->values = RedisModule_LoadDouble(io);
         return da;
    
     }
  
# allocate Memory, 記憶體配置

To store user keys, Redis allocates at most as much memory as the maxmemory setting enables (however there are small extra allocations possible).

The exact value can be set in the configuration file or set later via CONFIG SET (see Using memory as an LRU cache for more info). There are a few things that should be noted about how Redis manages memory:

     Redis will not always free up (return) memory to the OS when keys are removed.

This is not something special about Redis, but it is how most malloc() implementations work. 

For example if you fill an instance with 5GB worth of data, and then remove the equivalent of 2GB of data, the Resident Set Size (also known as the RSS, which is the number of memory pages consumed by the process) will probably still be around 5GB, even if Redis will claim that the user memory is around 3GB. This happens because the underlying allocator can't easily release the memory. For example often most of the removed keys were allocated in the same pages as the other keys that still exist.

The previous point means that you need to provision memory based on your peak memory usage. If your workload from time to time requires 10GB, even if most of the times 5GB could do, you need to provision for 10GB.

However allocators are smart and are able to reuse free chunks of memory, so after you freed 2GB of your 5GB data set, when you start adding more keys again, you'll see the RSS (Resident Set Size) to stay steady and don't grow more, as you add up to 2GB of additional keys. The allocator is basically trying to reuse the 2GB of memory previously (logically) freed.

Because of all this, the fragmentation ratio is not reliable when you had a memory usage that at peak is much larger than the currently used memory. The fragmentation is calculated as the amount of memory currently in use (as the sum of all the allocations performed by Redis) divided by the physical memory actually used (the RSS value). Because the RSS reflects the peak memory, when the (virtually) used memory is low since a lot of keys / values were freed, but the RSS is high, the ratio mem_used / RSS will be very high.

If maxmemory is not set Redis will keep allocating memory as it finds fit and thus it can (gradually) eat up all your free memory. Therefore it is generally advisable to configure some limit. You may also want to set maxmemory-policy to noeviction (which is not the default value in some older versions of Redis).

It makes Redis return an out of memory error for write commands if and when it reaches the limit - which in turn may result in errors in the application but will not render the whole machine dead because of memory starvation.




