//
//DELETE ME
//

function fromWebPage(result, name, dataUrl, filter) {
  Logger.log('Entered fromWebPage');
  
  var htmlData = fetchPage( dataUrl );
  var dateEntry = parseHtml( htmlData, filter );  

  result.push( itemStart, name + ' : ' + dateEntry );
}

function fetchPage(feed) {

  Logger.log('Entered dataUrl for : %s', feed);
  
  var html = UrlFetchApp.fetch(feed).getContentText();
  var doc = Xml.parse(html, true);
  var bodyHtml = doc.html.body.toXmlString();
  return XmlService.parse(bodyHtml).getRootElement();
}

function parseHtml( html, filter ) {

  Logger.log('Entered parseHtml');
  var data = getElementsByClassName(html, filter)[0];
  Logger.log("Found %s", data);
  //var output = XmlService.getRawFormat().format(data)
 
  return data;
}

function getElementsByClassName(element, classToFind) {  
  var data = [];
  var descendants = element.getDescendants();
  descendants.push(element);  
  for(i in descendants) {
    var elt = descendants[i].asElement();
    if(elt != null) {
      var classes = elt.getAttribute('class');
      if(classes != null) {
        classes = classes.getValue();
        if(classes == classToFind) data.push(elt.getText());
        else {
          classes = classes.split(' ');
          for(j in classes) {
            if(classes[j] == classToFind) {
              data.push(elt.getText());
              break;
            }
          }
        }
      }
    }
  }
  return data;
}

