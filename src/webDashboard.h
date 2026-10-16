#ifndef NERDMINER_WEB_DASHBOARD_H
#define NERDMINER_WEB_DASHBOARD_H

#include <Arduino.h>

struct WebDashboardSnapshot {
  bool wifiConnected;
  bool walletConfigured;
  char status[24];
  char ipAddress[16];
  char hashRate[20];
  char completedShares[20];
  char totalKHashes[20];
  char totalMHashes[20];
  char templates[20];
  char bestDifficulty[20];
  char timeMining[20];
  char valids[20];
  char temperature[12];
  char currentTime[12];
  char currentDate[16];
  char btcPrice[20];
  char blockHeight[16];
  char networkDifficulty[20];
  char globalHashRate[20];
  char halfHourFee[20];
  char remainingBlocks[24];
  float progressPercent;
  uint32_t workersCount;
  char workersHash[24];
  char poolBestDifficulty[20];
};

void startWebDashboard();
void handleWebDashboard();
void setWebDashboardSnapshot(const WebDashboardSnapshot &snapshot);

#endif
