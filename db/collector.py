import sqlite3
import FinanceDataReader as fdr

def STOCK_INFO_updater(db_connection : sqlite3.Connection, market : str):
    code_list = fdr.StockListing(market)
    cur = db_connection.cursor()
    query = 'INSERT INTO STOCK_INFO VALUES(:code, :name, :market, :ISU_CD, :sector);'
    if market in ['KRX', 'KOSPI', 'KOSDAQ', 'KONEX']:
        code2sector = krx_code_to_sector()
        for idx in range(len(code_list)):
            row = code_list.iloc[idx]
            try:
                cur.execute(query, {
                    'code': row['Code'], 'name': row['Name'], 'market': row['Market'],
                    'sector': (code2sector[row['Code']] if row['Code'] in code2sector else 'NULL'),
                    'ISU_CD': row['ISU_CD']
                })
            except:
                continue
    else:
        for idx in range(len(code_list)):
            row = code_list.iloc[idx]
            try:
                cur.execute(query, {
                    'code': row['Symbol'], 'name': row['Name'], 'market': market, 'sector': row['IndustryCode'],
                    'ISU_CD': 'NULL'
                })
            except:
                continue
    db_connection.commit()

def MARKET_updater(db_connection : sqlite3.Connection, market : str):
    import numpy as np
    from datetime import datetime
    start_date = '20180101'
    
    cur = db_connection.cursor()
    cur.execute('SELECT code FROM STOCK_INFO WHERE market="{}"'.format(market))
    code_list = cur.fetchall()
    for i in range(len(code_list)): # [(code1, ), (code2, ), ..] -> [code1, code2, ..]
        code_list[i] = code_list[i][0]
    
    cur.execute('SELECT MAX(date) FROM {};'.format(market))
    lastest_date = cur.fetchone()[0] # (date, )[0]
    if lastest_date is None: # No data, then start from '20150101'
        lastest_date = start_date

    for idx, code in enumerate(code_list):
        print('Updating.. {} ({}/{})\r'.format(code, idx+1, len(code_list)), end='')
        try:
            data = fdr.DataReader(code, lastest_date)
            data['Change'] = [0] + (np.array(data['Close'][1:]) - np.array(data['Close'][:-1])).tolist()
            if market in ['KOSPI', 'KOSDAQ']:
                cur.execute('SELECT ISU_CD FROM STOCK_INFO WHERE code="{}"'.format(code))
                isu_cd = cur.fetchone()[0] # (ISU_CD, )[0]
                per_pbr = krx_per_pbr(lastest_date, str(datetime.today().date()).replace('-', ''), isu_cd)
            if len(per_pbr) != len(data):
                diff = len(data) - len(per_pbr)
                to_concat = pd.DataFrame({'PER': [0 for _ in range(diff)], 'PBR': [0 for _ in range(diff)]})
                per_pbr = pd.concat([to_concat, per_pbr], ignore_index=True)
        except:
            continue

        query = 'INSERT INTO {} VALUES(:date, :code, :open, :high, :low, :close, :volume, :change, :per, :pbr)'.format(market)
        for i in range(len(data)):
            row = data.iloc[i]
            if market in ['KOSPI', 'KOSDAQ']:
                cur.execute(query, {
                    'date': str(row.name.date()).replace('-', ''),
                    'code': code, 'open': int(row['Open']), 'high': int(row['High']), 'low': int(row['Low']), 'close': int(row['Close']),
                    'volume': int(row['Volume']), 'change': int(row['Change']), 'per': float(per_pbr.iloc[i]['PER']), 'pbr': float(per_pbr.iloc[i]['PBR'])
                })
            else:
                cur.execute(query, {
                    'date': str(row.name.date()).replace('-', ''),
                    'code': code, 'open': float(row['Open']), 'high': float(row['High']), 'low': float(row['Low']), 'close': float(row['Close']),
                    'volume': int(row['Volume']), 'change': float(row['Change']), 'per': 'NULL', 'pbr': 'NULL'
                })
    db_connection.commit()



    

import requests as rq
from io import BytesIO
import pandas as pd
gen_otp_url = "http://data.krx.co.kr/comm/fileDn/GenerateOTP/generate.cmd"
download_url = 'http://data.krx.co.kr/comm/fileDn/download_csv/download.cmd'
header = {'Referer': 'http://data.krx.co.kr/contents/MDC/MDI/mdiLoader'}

def krx_code_to_sector():
    otp_data = {
        'locale': 'ko_KR',
        'mktTpCd': '0',
        'tboxisuSrtCd_finder_listisu0_2': '전체',
        'isuSrtCd': 'ALL',
        'isuSrtCd2': 'ALL',
        'codeNmisuSrtCd_finder_listisu0_2': '',
        'param1isuSrtCd_finder_listisu0_2': '',
        'sortType': 'I',
        'stdIndCd': 'ALL',
        'sectTpCd':'ALL',
        'parval': 'ALL',
        'mktcap': 'ALL',
        'acntclsMm': 'ALL',
        'tboxmktpartcNo_finder_designadvser0_2': '',
        'mktpartcNo': '',
        'mktpartcNo2': '',
        'codeNmmktpartcNo_finder_designadvser0_2': '',
        'param1mktpartcNo_finder_designadvser0_2': '',
        'condListShrs': '1',
        'listshrs': '',
        'listshrs2': '',
        'condCap': '1',
        'cap': '',
        'cap2': '',
        'share': '1',
        'money': '1',
        'csvxls_isNo': 'false',
        'name': 'fileDown',
        'url': 'dbms/MDC/STAT/standard/MDCSTAT03402',
    }
    otp = rq.post(gen_otp_url, otp_data, headers=header).text
    download_csv = rq.post(download_url, {'code': otp}, headers=header)
    csv_to_pd = pd.read_csv(BytesIO(download_csv.content), encoding='cp949')
    code_to_sector = dict()
    for i in range(len(csv_to_pd)):
        code = str(csv_to_pd.iloc[i]['종목코드']).zfill(6)
        sector = str(csv_to_pd.iloc[i]['업종코드']).zfill(6)
        code_to_sector[code] = sector
    return code_to_sector


def krx_per_pbr(start_date : str, end_date : str, isu_cd : str):
    otp_data = {
        'mktId': 'ALL',
        'url': 'dbms/MDC/STAT/standard/MDCSTAT03502',
    }
    otp_data['isuCd'] = isu_cd
    otp_data['strtDd'] = start_date
    otp_data['endDd'] = end_date
    otp = rq.post(gen_otp_url, otp_data, headers=header).text
    download_csv = rq.post(download_url, {'code': otp}, headers=header)

    csv_to_pd = pd.read_csv(BytesIO(download_csv.content), encoding='EUC-KR')
    csv_to_pd = csv_to_pd.fillna(-1)
    csv_to_pd = csv_to_pd[(csv_to_pd['PER'] != -1.) & (csv_to_pd['PBR'] != -1.)]
    return csv_to_pd[['PER', 'PBR']].sort_index(ascending=False)