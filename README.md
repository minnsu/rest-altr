# Algorithm Trading System with KIS Developers

## Dependencies
- Boost
    - asio 1.28.2 - https request
    - beast - Websocket
    - json - for parsing and restruct http packet
- Python3 
    - pandas, numpy, bs4
    - finance-datareader - for stock code to market transform
        - ```pip install finance-datareader```
- Others
    - libcrypto++-dev - for decrypt realtime data
        - ```apt install libcrypto++-dev```
    - sqlite3
        - ```apt install sqlite3 libsqlite3-dev```


## How to run
1. Collect(or Update) data.
    - project/db/initializer.py initialize the database "stocks.db" table.
    - project/db/collector.py have functions that collect data from web and store to "stocks.db".
        - ```STOCK_INFO_updater```: Read stock list from FinanceDataReader and store each stock's information to database.
        - ```MARKET_updater```: Read data of each stocks in specified market from web. Currently, only KOSPI, KOSDAQ are supported.
2. Compile the source code with proper project/src/main.cpp
    - Real System vs. Back Testing
3. Backtesting
    - You should implements the backtesting strategy function in project/src/include/runtime/runtime.cpp.
    - Some useful indicator are provided to implements strategy.
4. Real transaction
    - Change the main code to execute transaction.
    - Strategy that you implemented is maintained.

## For new API Trading System
1. If you need your own backtesting data, then you should write appropriate data collecting code and database.
2. Implement or change the code of project/api/api.hpp and its children source codes for supporting new API.
3. And connect to project/src/include/engine/* to work properly.