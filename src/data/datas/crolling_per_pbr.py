import requests as rq
from io import BytesIO
import numpy as np
import pandas as pd

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

def moving_avg(days, avg_accum, code):
    return np.mean(avg_accum[code][-days:])

def rsi(days, rsi_accum, code):
    up_list = [e for e in rsi_accum[code][-days:] if e > 0]
    down_list = [e for e in rsi_accum[code][-days:] if e < 0]
    avg_up = sum(up_list) / (len(up_list) + 1e-8)
    avg_down = sum(down_list) / (len(down_list) + 1e-8)
    return avg_up / (avg_up - avg_down + 1e-8)

def bollinger_band(avg_accum, code, days=20, std_factor=2):
    avg = np.mean(avg_accum[code][-days:])
    std = np.std(avg_accum[code][-days:])
    return (avg - std * std_factor, avg + std * std_factor)


from datetime import datetime
import datetime as dt

file_name = 'kospi_2023.txt'
start_date = datetime(2023, 1, 1).date()
end_date = datetime.now().date()
# end_date = datetime(2022, 12, 31).date()
interval = dt.timedelta(days=1)


stocks = dict()
avg_accum = dict()
rsi_accum = dict()

date = start_date - interval * 90 # for indicator calculate
while date <= end_date:
    print('{} ({} ~ {})\r'.format(date, start_date, end_date), end='')
    date += interval
    
    date_str = str(date).replace('-', '')
    csv = croll_per_pbr_csv(date_str)
    if len(csv) == 0:
        continue
    print(csv)
    if date >= start_date:
        stocks[date_str] = dict()
    for i in range(len(csv)):
        code = csv.iloc[i, 0]
        if code not in rsi_accum:
            rsi_accum[code] = []
            avg_accum[code] = []
            rsi_accum[code] += [csv.iloc[i, 3]] # 대비: 증감가격
            avg_accum[code] += [csv.iloc[i, 2]] # 종가
        else:
            rsi_accum[code] += [csv.iloc[i, 3]]
            avg_accum[code] += [csv.iloc[i, 2]]
        
        if date < start_date:
            continue
        avg5 = moving_avg(5, avg_accum, code)
        avg20 = moving_avg(20, avg_accum, code)
        avg60 = moving_avg(60, avg_accum, code)
        rsi9 = rsi(9, rsi_accum, code)
        rsi14 = rsi(14, rsi_accum, code)
        rsi28 = rsi(28, rsi_accum, code)
        lbb, ubb = bollinger_band(avg_accum, code)
        stocks[date_str][code] = csv.iloc[i, [2,5,6,9,10]].tolist() + [avg5, avg20, avg60, rsi9, rsi14, rsi28, lbb, ubb]

f = open(file_name, 'w')
f.write(str(stocks).replace("'", '"'))
f.close()