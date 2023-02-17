from yahoo_fin import stock_info as si
import sys

tickers = open(sys.argv[0], 'r').readlines()

i = 0
while i < len(tickers):
    tickers[i]= tickers[i][0:-1]

    quote_table = []
    try:
        quote_table = si.get_data(tickers[i], start_date="1/1/2020", end_date="3/1/2022", interval="1d", index_as_date=False)
    except:
        print(f'ticker: {tickers[i]} unavailable')
    
    if len(quote_table) > 50:
        print(f'{tickers[i]} {i}')
        file = open(f'{sys.argv[1]}/{tickers[i]}.txt', 'w')
        for price in quote_table['open']:
            file.write(f'{str(price)}\n')
        for price in quote_table['close']:
            file.write(f'{str(price)}\n')
        file.close()
    else:
        tickers.pop(i)
        i-=1
    
    i+=1

file = open(sys.argv[0], 'w')
for i in tickers:
    file.write(f'{i}\n')
