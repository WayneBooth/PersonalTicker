
// Stock price || https://api.iextrading.com/1.0/stock/MA/stats  (https://iextrading.com/developer/docs/)
function getStocks( result, name, symbols ) {
  Logger.log('Entered getStocks');
  result.push( itemStart, name + ' : ');
  
  var data = [];
  for (var count in symbols) {
    var feed = 'https://api.iextrading.com/1.0/stock/' + symbols[count];
    var price = fetchExternal(feed + '/price', 3600);
    var json = fetchExternal(feed + '/stats', 3600);
    var stock_data = JSON.parse(json);
    result.push( itemStart, stock_data.companyName + ' (' + symbols[count] + ') : ' + price 
              + ' (5 day change ' + (stock_data.day5ChangePercent*100).toFixed(2) + '%, '
              + '30 day change ' + (stock_data.day30ChangePercent*100).toFixed(2) + '%)');
  }

}