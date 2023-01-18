This is a project use Raft to sync data between each leveldb.
### Before Build 
You should have boost in your machine.
```
${BOOST_INCLUDE_PATH} AND ${BOOST_LIBRARY_PATH}  should in env

epxort BOOST_INCLUDE_PATH = ${BOOST_INCLUDE_PATH}

export BOOST_LIBRARY_PATH = ${BOOST_LIBRARY_PATH} 
```
###  Clone and Build
```
git clone https://github.com/white-cc/leveldb-raft.git

git submodule update --init  --recursive 

mkdir build && cd build

cmake .. -DUSE_RAFT_SYNC = 1  && make -j 
```

### How to use

you can use as a submodule in your project
```
lib ${LEVELDB_SYNC_LIB} 
include_path ${LEVELDB_PUBLIC_INCLUDE_DIR}
```
or exec leveldb_raft for test
```
build/leveldb_raft ${sever_number} ${ip}:${port}

like

build/leveldb_raft 1 127.0.0.1:10001
```
add server into your raft group
```
> add 127.0.0.1:10001
```
list raft group
```
> ls
```
More help
```
> help
```