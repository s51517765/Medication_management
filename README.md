# Medication_management

## 概要 (Overview)

お薬を飲んだ時刻を記録します。<br>
3つのボタンにそれぞれお薬を割り当てることが出来ます。<br>
記録はGAS経由でGoogleスプレッドシートに記録しています。<br>
また、最後の服薬記録を参照し、服薬時刻と経過時間を表示します。

## 画面イメージ (ScreenShot)

<img src="https://github.com/s51517765/Medication_management/blob/main/image.jpg">

## 動作要件 (Requirements)

M5Stack Basic / C++ / GAS(Google app script) / Googleスプレッドシート

## 技術的解説 (Technical explanation)

https://s51517765.hatenadiary.jp/entry/2021/05/10/073000

## 回路図 (Circuit diagram)

NA

## ライセンス (License)

This software is released under the MIT License, see LICENSE.

主要コードはsrc/main.cppとgas/main.gsのみです。その他はPlatformIOが生成したファイルやprojectファイルなど。