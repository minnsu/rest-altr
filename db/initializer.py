import sqlite3

db_path = './stocks.db'

conn = sqlite3.connect(db_path)
cur = conn.cursor()

# STOCK_INFO
cur.execute('CREATE TABLE IF NOT EXISTS STOCK_INFO(code TEXT, name TEXT, market TEXT, ISU_CD TEXT, sector TEXT, PRIMARY KEY(code, name));')
cur.execute('CREATE INDEX code_name ON STOCK_INFO (code, name);')
# Markets

# Korea
cur.execute('CREATE TABLE IF NOT EXISTS KOSPI(date TEXT, code TEXT, open INTEGER, high INTEGER, low INTEGER, close INTEGER, volume INTEGER, change INTEGER, per REAL, pbr REAL, PRIMARY KEY(date, code));')
cur.execute('CREATE INDEX i_all_1 ON KOSPI (date, code, open, high, low, close, volume, change, per, pbr);')
cur.execute('CREATE TABLE IF NOT EXISTS KOSDAQ(date TEXT, code TEXT, open INTEGER, high INTEGER, low INTEGER, close INTEGER, volume INTEGER, change INTEGER, per REAL, pbr REAL, PRIMARY KEY(date, code));')
cur.execute('CREATE INDEX i_all_2 ON KOSDAQ (date, code, open, high, low, close, volume, change, per, pbr);')

# US
cur.execute('CREATE TABLE IF NOT EXISTS NASDAQ(date TEXT, code TEXT, open INTEGER, high INTEGER, low INTEGER, close INTEGER, volume INTEGER, change INTEGER, per REAL, pbr REAL, PRIMARY KEY(date, code));')
cur.execute('CREATE INDEX i_all_3 ON NASDAQ (date, code, open, high, low, close, volume, change, per, pbr);')
cur.execute('CREATE TABLE IF NOT EXISTS NYSE(date TEXT, code TEXT, open INTEGER, high INTEGER, low INTEGER, close INTEGER, volume INTEGER, change INTEGER, per REAL, pbr REAL, PRIMARY KEY(date, code));')
cur.execute('CREATE INDEX i_all_4 ON NYSE (date, code, open, high, low, close, volume, change, per, pbr);')
cur.execute('CREATE TABLE IF NOT EXISTS AMEX(date TEXT, code TEXT, open INTEGER, high INTEGER, low INTEGER, close INTEGER, volume INTEGER, change INTEGER, per REAL, pbr REAL, PRIMARY KEY(date, code));')
cur.execute('CREATE INDEX i_all_5 ON AMEX (date, code, open, high, low, close, volume, change, per, pbr);')