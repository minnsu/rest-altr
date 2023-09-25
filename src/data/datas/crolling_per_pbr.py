import requests as rq
from io import BytesIO
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


from datetime import datetime
import datetime as dt

file_name = 'kospi_2023.txt'
start_date = datetime(2023, 9, 1).date()
end_date = datetime.now().date()
# end_date = datetime(2023, 12, 31).date()
interval = dt.timedelta(days=1)


stocks = dict()

date = start_date
while date <= end_date:
    print('{} ({} ~ {})\r'.format(date, start_date, end_date), end='')
    csv = croll_per_pbr_csv(str(date).replace('-', ''))
    date += interval
    if len(csv) == 0:
        continue
    # print(csv.loc[:, ['종목코드', '종가', 'EPS', 'PER', 'BPS', 'PBR']])
    stocks[str(date).replace('-', '')] = dict()
    for i in range(len(csv)):
        code = csv.iloc[i, 0]
        stocks[str(date).replace('-', '')][code] = csv.iloc[i, [2,5,6,9,10]].tolist()

f = open(file_name, 'w')
f.write(str(stocks))
f.close()