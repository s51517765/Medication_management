function doPost(e) {  
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName('シート1');
  var params = JSON.parse(e.postData.getDataAsString());
  var id = params.id;

if(id=="0"){
  // データをシートに追加
    var range = sheet.getRange("A2");
  range.insertCells(SpreadsheetApp.Dimension.ROWS);
  sheet.getRange(2, 1).setValue(new Date());     // 受信日時を記録
}
else if(id=="1"){
       var range = sheet.getRange("B2");
  range.insertCells(SpreadsheetApp.Dimension.ROWS);
  sheet.getRange(2, 2).setValue(new Date());     // 受信日時を記録
}
else if(id=="2"){
      var range = sheet.getRange("C2");
  range.insertCells(SpreadsheetApp.Dimension.ROWS);
  sheet.getRange(2, 3).setValue(new Date());     // 受信日時を記録
}
  // 最新行の日時セル内容を取得してフォーマット
  var targetDate0 = sheet.getRange(2,1).getValue();
  var dateString0 = "";
  if (targetDate0 != null){
    dateString0 = Utilities.formatDate(targetDate0,"JST","MM/dd HH:mm");
  }  
  var targetDate1 = sheet.getRange(2,2).getValue();
  var dateString1 = "";
  if (targetDate1 != null){
    dateString1 = Utilities.formatDate(targetDate1,"JST","MM/dd HH:mm");
  }  
  var targetDate2 = sheet.getRange(2,3).getValue();
  var dateString2 = "";
  if (targetDate2 != null){
    dateString2 = Utilities.formatDate(targetDate2,"JST","MM/dd HH:mm");
  }  
  
  // レスポンス
  var response = {
    data: { "0": dateString0 ,"1":dateString1,"2":dateString2},
    past:{ "0": get_history(0) ,"1":get_history(1),"2":get_history(2)}
  };
  return ContentService.createTextOutput(JSON.stringify(response)).setMimeType(ContentService.MimeType.JSON);
}

function get_history(num){
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getSheetByName('シート1');
  var range;
  if(num==0)  range = sheet.getRange("A2");
  else if(num==1)  range = sheet.getRange("B2");
  else if(num==2)  range = sheet.getRange("C2");

  var value = range.getValue();
  var lasttime =Utilities.formatDate(value,"JST","MM/dd HH:mm")
  var timeDelta = new Date()-value;
  Logger.log(lasttime);
  var h =parseInt(timeDelta/1000/60/60);
  var d = parseInt(h/24);
  var res;
  if(d>0) res = lasttime.toString() + "("+d.toString()+"days before)";
  else res = lasttime.toString() + "("+h.toString()+"h before)";
  Logger.log(res);

  return res;
}