# My TTGo Watch

It's a dumb-watch with a pong face background. Press button to wake. Battery life approx 3.5 days assuming normal usage.

## Minimal framework for FreeRTOS tasks

Actors. General classes which have a display function. They each get their own task, and expose a `run()` `init()` and `display()` method. The display worker has to be manually coded to call on added actors.

## No touch interface

There's no interactivity to this watch. It tells the time, and shows the date. The code to poll Wifi to fetch from NTP exists but is disabled, and the time zone is hard coded. The RTC is good enough that syncing from NTP doesn't need to happen on any regular schedule.

## Power consumption

The power manager class has both a short power down and a deep power down. Short is essentially a screen-off. Deep is esp_light_sleep, with wake from the system button interrupt only. Short power down will wake on tft touch.

The watch will last over a weekend on battery without a charge. A 3-day weekend is pushing it. Also don't check the time that often... it doesn't do anything besides the time, why are you looking at that often?
