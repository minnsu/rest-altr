import requests as rq
from io import BytesIO
import numpy as np
import pandas as pd
import FinanceDataReader as fdr

gen_otp_url = "http://data.krx.co.kr/comm/fileDn/GenerateOTP/generate.cmd"
download_url = 'http://data.krx.co.kr/comm/fileDn/download_csv/download.cmd'

header = {'Referer': 'http://data.krx.co.kr/contents/MDC/MDI/mdiLoader'}
default_otp_data = {
    # 'locale': 'ko_KR',
    # 'searchType': '1',
    'mktId': 'ALL',

    # 'tboxisuCd_finder_stkisu0_0': '006840/AK홀딩스',
    # 'isuCd': 'KR7006840003',
    # 'isuCd2': 'KR7005930003',
    # 'codeNmisuCd_finder_stkisu0_0': 'AK홀딩스',
    # 'param1isuCd_finder_stkisu0_0': 'ALL',
    # 'strDd': '20230915',
    # 'endDd': '20230922',
    # 'csvxls_isNo': 'false',
    # 'name': 'fileDown',
    'url': 'dbms/MDC/STAT/standard/MDCSTAT03501',
}

def croll_per_pbr_csv(date : str):
    otp_data = default_otp_data.copy()
    otp_data['trdDd'] = date
    otp = rq.post(gen_otp_url, otp_data, headers=header).text
    download_csv = rq.post(download_url, {'code': otp}, headers=header)
    csv_to_pd = pd.read_csv(BytesIO(download_csv.content), encoding='EUC-KR')
    csv_to_pd = csv_to_pd.fillna(-1)
    csv_to_pd = csv_to_pd[(csv_to_pd['PER'] != -1.) & (csv_to_pd['PBR'] != -1.)]
    return csv_to_pd

def moving_avg(days, price_accum, idx):
    return np.mean(price_accum[max(idx - days + 1, 0):idx + 1])

def rsi(days, diff_accum, idx):
    up_list = [e for e in diff_accum[max(idx - days + 1, 0):idx + 1] if e > 0]
    down_list = [e for e in diff_accum[max(idx - days + 1, 0):idx + 1] if e < 0]
    avg_up = sum(up_list) / (len(up_list) + 1e-8)
    avg_down = sum(down_list) / (len(down_list) + 1e-8)
    return avg_up / (avg_up - avg_down + 1e-8)

def bollinger_band(price_accum, idx, days=20, std_factor=2):
    avg = np.mean(price_accum[max(idx - days + 1, 0):idx + 1])
    std = np.std(price_accum[max(idx - days + 1, 0):idx + 1])
    return (avg - std * std_factor, avg + std * std_factor)


from datetime import datetime
import datetime as dt

file_name = 'krx_2023.txt'
start_date = datetime(2023, 1, 2).date() # Must not be holiday
end_date = datetime.now().date()
# end_date = datetime(2022, 12, 31).date()
interval = dt.timedelta(days=1)

date = start_date - interval * 90

price_accum = dict()
diff_accum = dict()
accum_idx_dict = dict()

krx_list = fdr.StockListing('KRX')['Code'].tolist()
for idx, code in enumerate(krx_list):
    print('Downloading.. {} [{}/{}] {}%\r'.format(code, idx, len(krx_list), int(100 * (idx + 1) / len(krx_list))), end='')
    load_from_fdr = fdr.DataReader(code, str(date), str(end_date))
    price_accum[code] = np.array(load_from_fdr['Close'].tolist())
    diff_accum[code] = np.array(price_accum[code]) * np.array(load_from_fdr['Change'].tolist())
    try:
        accum_idx_dict[code] = (load_from_fdr.index == str(start_date)).tolist().index(True)
    except:
        accum_idx_dict[code] = 0
print('Done.')

stocks = dict()
date = start_date
while date <= end_date:
    print('{} ({} ~ {})\r'.format(date, start_date, end_date), end='')
    date_str = str(date).replace('-', '')

    csv = croll_per_pbr_csv(date_str)
    if len(csv) == 0: # no stock data in date
        date += interval
        continue
    
    # print('Date: ', date)
    # print(csv)
    
    stocks[date_str] = dict()
    for i in range(len(csv)):
        code = csv.iloc[i, 0]
        try:
            accum_idx = accum_idx_dict[code]
        except:
            continue
        
        avg5 = moving_avg(5, price_accum[code], accum_idx)
        avg20 = moving_avg(20, price_accum[code], accum_idx)
        avg60 = moving_avg(60, price_accum[code], accum_idx)
        rsi9 = rsi(9, diff_accum[code], accum_idx)
        rsi14 = rsi(14, diff_accum[code], accum_idx)
        rsi28 = rsi(28, diff_accum[code], accum_idx)
        lbb, ubb = bollinger_band(price_accum[code], accum_idx)
        stocks[date_str][code] = [price_accum[code][accum_idx]] + csv.iloc[i, [6,10]].tolist() + [avg5, avg20, avg60, rsi9, rsi14, rsi28, lbb, ubb]
        # print(stocks[date_str][code])
        accum_idx_dict[code] += 1
    
    date += interval

f = open(file_name, 'w')
f.write(str(stocks).replace("'", '"'))
f.close()