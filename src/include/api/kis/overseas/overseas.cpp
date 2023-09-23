#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>

#include "../../api.hpp"

using namespace api::kis;

string overseas::market_code[] = {
    // us
    "NASD", // nasdaq
    "NYSE", // newyork
    "AMEX", // amex
    // japan
    "TKSE",
    // hongkong
    "SEHK",
    // china
    "SHAA", // shanghai
    "SZAA", // simchun
    // vietnam
    "VNSE", // hochimin
    "HASE", // hanoi
};

string overseas::exchan_code[] = {
    // us
    "NAS",
    "NYS",
    "AMS",
    // japan
    "TSE",
    // hongkong
    "HKS",
    // china
    "SHS",
    "SZS",
    // vietnam
    "HSX", // hochiminh
    "HNX", // hanoi

    // china index
    "SHI", // shanghai index
    "SZI", // simchun index

    // us after close
    "BAQ", // nasdaq
    "BAY", // newyork
    "BAA", // amex
};

bool load_codes = false;
// nasd, nyse, amex, tkse, sehk, shaa, szaa, vnse
vector<set<string>> codes(8);
// there is no hanoi stock list

/**
 * Load code list files to set.
*/
void load2set() {
    string path, tmp;
    for(int i = 0; i < 8; i++) {
        path = "./src/data/codes/" + overseas::market_code[i] + ".txt";
        ifstream file(path);
        if(!file.is_open()) {            
            cerr << "Can not find " + path + "Currently executing in " + filesystem::current_path().string() << endl;
            exit(1);
        }
        while(!file.eof()) {
            file >> tmp;
            codes[i].insert(tmp);
        }
        file.close();
    }
}

/**
 * transform stock code to proper market number.
 * @param {string&} code: target stock code.
*/
overseas::market overseas::code2market(string& code) {
    if(!load_codes) {
        load2set();
        load_codes = true;
    }
    for(int i = 0; i < 8; i++) {
        if(codes[i].find(code) != codes[i].end()) {
            switch(i) {
            case 0:
                return overseas::market::us_nasd;
            case 1:
                return overseas::market::us_nyse;
            case 2:
                return overseas::market::us_amex;
            case 3:
                return overseas::market::jp_tkse;
            case 4:
                return overseas::market::hk_sehk;
            case 5:
                return overseas::market::ch_shaa;
            case 6:
                return overseas::market::ch_szaa;
            case 7:
                return overseas::market::vn_vnse;
            }
        }
    }
    return overseas::market::vn_hase;
}