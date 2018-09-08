function fetchExternal(url, length) {
  return fetchExternalHeader(url, length, null);
}

function fetchExternalHeader(url, length, header) {

  var id = Utilities.base64Encode(url);
  var cache = CacheService.getScriptCache();
  var cached = cache.get(id);
  if (cached != null) {
    //Logger.log("returned cached value : %s", cached);
    return cached;
  }
  
  var externalPage;
  if(header != null) {
    //Logger.log("Getting URL with headers");
    var options = {};
    options.headers = header;
    externalPage = UrlFetchApp.fetch(url, options);
  }
  else {
    //Logger.log("Getting URL withOUT headers : %s", url);
    externalPage = UrlFetchApp.fetch(url);
  }
  
  var contents = externalPage.getContentText();
  //Logger.log("returned LIVE value : %s", contents);
  cache.put(id, contents, length);
  return contents;
}
