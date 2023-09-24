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

start_date = datetime(2020, 6, 1).date()
end_date = datetime.now().date()
interval = dt.timedelta(days=1)

date = start_date
while date <= end_date:
    print(date)
    csv = croll_per_pbr_csv(str(date).replace('-', ''))
    date += interval
    if len(csv) == 0:
        continue
    print(csv)