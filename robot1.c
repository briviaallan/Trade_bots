//+------------------------------------------------------------------+
//|                                                       robot1.mq4 |
//|                        Copyright 2023, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property strict
//+------------------------------------------------------------------+
//| Script program start function                                    |
//+------------------------------------------------------------------+
#include <string>
#include <math.h>

int fastKPeriod = 14;
int slowKPeriod = 3;
int emaPeriod = 20;
double maxRiskPercentage = 2.0;
double stopLossPips = 30;
double takeProfitPips = 50;
double lotSize = 0.01;
double minimumLotSize = 0.01;
double ask;
double bid;
int symbolDigits = 5;

double selectLotSize(double accountBalance, double maxRiskPercentage, double stopLossPips, double ask) {
  double tradeValue = accountBalance * maxRiskPercentage / 100;
  double stopLossPrice = ask - stopLossPips * pow(10, -symbolDigits);
  double lotSize = tradeValue / (ask - stopLossPrice);
  lotSize = lotSize - fmod(lotSize, minimumLotSize);
  return lotSize;
}

void buy_call() {
  ask = MarketInfo(Symbol(), MODE_ASK);
  lotSize = selectLotSize(AccountBalance(), maxRiskPercentage, stopLossPips, ask);
  int ticket = OrderSend(Symbol(), OP_BUY, lotSize, ask, 3, ask - stopLossPips * pow(10, -symbolDigits), ask + takeProfitPips * pow(10, -symbolDigits), "Buy Order", 16384, 0, clrGreen);
  if (ticket < 0) {
    Print("Error opening BUY order : ", GetLastError());
  }
}

void sell_call() {
  bid = MarketInfo(Symbol(), MODE_BID);
  lotSize = selectLotSize(AccountBalance(), maxRiskPercentage, stopLossPips, bid);
  int ticket = OrderSend(Symbol(), OP_SELL, lotSize, bid, 3, bid + stopLossPips * pow(10, -symbolDigits), bid - takeProfitPips * pow(10, -symbolDigits), "Sell Order", 16384, 0, clrRed);
  if (ticket < 0) {
    Print("Error opening SELL order : ", GetLastError());
  }
}

void OnTick() {
  double fastK = iStochastic(NULL, 0, fastKPeriod, slowKPeriod, MODE_SMA, STO_LOWHIGH, 1);
  double ema = iMA(NULL, 0, emaPeriod, 0, MODE_EMA, PRICE_CLOSE, 0);
  double price = Close[0];

  if (fastK < 20 && ema > price) {
    sell_call();
  } else if (fastK > 80 && ema < price) {
    buy_call();
  }
}
