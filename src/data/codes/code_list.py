import FinanceDataReader as fdr

def list2file(file, stock_list):
    f = open(file+'.txt', 'w')
    for symbol in stock_list['Symbol']:
        f.write(symbol + '\n')
    f.close()

# korea
kospi = fdr.StockListing('KOSPI')
f = open('KOSPI.txt', 'w')
for code in kospi['Code']:
    f.write(code+'\n')
f.close()
kosdaq = fdr.StockListing('KOSDAQ')
f = open('KOSDAQ.txt', 'w')
for code in kosdaq['Code']:
    f.write(code+'\n')
f.close()

# us
nasdaq = fdr.StockListing('NASDAQ')
list2file('NASD', nasdaq)
newyork = fdr.StockListing('NYSE')
list2file('NYSE', newyork)
amex = fdr.StockListing('AMEX')
list2file('AMEX', amex)

# japan
tokyo = fdr.StockListing('TSE')
list2file('TKSE', tokyo)

# hongkong
hongkong = fdr.StockListing('HKEX')
list2file('SEHK', hongkong)

# china
shanghai = fdr.StockListing('SSE')
list2file('SHAA', shanghai)
simchun = fdr.StockListing('SZSE')
list2file('SZAA', simchun)

# vietnam
hochiminh = fdr.StockListing('HOSE')
list2file('VNSE', hochiminh)