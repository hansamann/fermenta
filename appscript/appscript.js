function setup() {
  console.log("OK");
}

function doPost(e){
  return handleResponse(e);
  /*if (e.parameter.code == '<password_here>') {
    return handleResponse(e);
  } else 
        return ContentService
          .createTextOutput(JSON.stringify({"result":"error", "error":"Access denied."}))
          .setMimeType(ContentService.MimeType.JSON);
  */
}

function handleResponse(e) {

  try {
    // next set where we write the data - you could write to multiple/alternate destinations
    var doc = SpreadsheetApp.openById("<id of spreadsheet>");
    var sheet = doc.getSheetByName("Log"); 
    
    var nextRow = sheet.getLastRow()+1; // get next row
    
    var row = [];
    var jsonData = JSON.parse(e.postData.contents);
    
    var d = new Date(jsonData.metadata.time);
    var dateFormatted = new Intl.DateTimeFormat('en-US', {dateStyle: 'short', timeZone : 'Europe/Berlin'}).format(d);
    var timeFormatted = new Intl.DateTimeFormat('en-US', {timeStyle: 'short', hour12:false, timeZone : 'Europe/Berlin'}).format(d);
    
    row.push(jsonData.metadata.time);
    row.push(dateFormatted);
    row.push(timeFormatted);
    row.push(jsonData.payload_fields.counter);
    row.push(jsonData.payload_fields.interval);
    row.push(jsonData.payload_fields.temp);
    sheet.getRange(nextRow, 1, 1, row.length).setValues([row]);

  
    
    // return json success results
    return ContentService
          .createTextOutput(JSON.stringify({"result":"success"}))
          .setMimeType(ContentService.MimeType.JSON);
  } catch(e) {
    // if error return this
    return ContentService
          .createTextOutput(JSON.stringify({"result":"error", "error": e}))
          .setMimeType(ContentService.MimeType.JSON);
  } 
}