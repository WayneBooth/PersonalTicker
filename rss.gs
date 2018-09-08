function getRss(result, name, maxItems, dataUrl) {
  Logger.log('Entered getRss');

  result.push( blockStart, name );
  
  var rssData = fetchFeed( dataUrl );
  parseFeed( result, rssData, maxItems, 1 );
}

function fetchFeed(feed) {

  Logger.log('Entered fetchFeed for : %s', feed);

  var txt = fetchExternal(feed, 3600);
  var doc = Xml.parse(txt, false);  
  
  return doc;
}

function parseFeed( result, doc, maxItems, full ) {

  var data = [];
  var title = doc.getElement().getElement("channel").getElement("title").getText();
  var items = doc.getElement().getElement("channel").getElements("item");   
  
  // Parsing single items in the RSS Feed
  for (var i in items) {

    try {
      
      item  = items[i];
      
      title = item.getElement("title").getText();
      desc = item.getElement("description").getText();
      //Logger.log( "Fouond %s entry of : %s", title, title + ' : ' + desc );

      if( full == 1 ) {
        result.push( itemStart, title + ' : ' + desc );
        full = 0;
      }
      else {
        result.push( itemStart, title );
      }
      
    } catch (e) {
      Logger.log(e);
    }
    
    if( --maxItems <= 0 ) {
      break;
    }
    
  }

}