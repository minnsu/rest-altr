import FinanceDataReader as fdr

def list2file(file, stock_list):
    f = open(file+'.txt', 'w')
    for symbol in stock_list['Symbol']:
        f.write(symbol + '\n')
    f.close()

# korea
# kospi = fdr.StockListing('KOSPI')
# kosdaq = fdr.StockListing('KOSDAQ')

# us
nasdaq = fdr.StockListing('NASDAQ')
newyork = fdr.StockListing('NYSE')
amex = fdr.StockListing('AMEX')

# japan
tokyo = fdr.StockListing('TSE')

# hongkong
hongkong = fdr.StockListing('HKEX')

# china
shanghai = fdr.StockListing('SSE')
simchun = fdr.StockListing('SZSE')

# vietnam
hochiminh = fdr.StockListing('HOSE')

list2file('NASD', nasdaq)
list2file('NYSE', newyork)
list2file('AMEX', amex)
list2file('TKSE', tokyo)
list2file('SEHK', hongkong)
list2file('SHAA', shanghai)
list2file('SZAA', simchun)
list2file('VNSE', hochiminh)