
function getCurrency( result, name, symbols ) {
  Logger.log('Entered getCurrency');
  result.push( itemStart, name + ' : ');

  var feed = 'https://free.currencyconverterapi.com/api/v6/convert'
    + '?q=GBP_USD,GBP_EUR'
  var price_json = fetchExternal(feed, 3600);
  var currency_data = JSON.parse(price_json);
  //result.push( itemStart, price_json );
  
  for (var count in currency_data.results) {
    result.push( itemStart, currency_data.results[count].fr 
      + ' -> ' + currency_data.results[count].to 
      + ' = ' + currency_data.results[count].val );
  }

}
