
function getMetalsPrice(result, name) {
  Logger.log('Entered getFinantial');
  result.push( itemStart, name + ' : ');
  
  var feed = 'http://goldpricez.com/api/rates/currency/gbp/measure/ounce';
  var price_json = fetchExternalHeader(feed, 3600, { 'X-API-KEY': API_KEY } );
  price_json = price_json.replace(/"\{/g,'{')
  price_json = price_json.replace(/\}"/g,'}')
  price_json = price_json.replace(/\\/g,'');

  var metals_data = JSON.parse(price_json);
  result.push( itemStart, "£" + metals_data.ounce_in_gbp 
              + ' @ ' + metals_data.gmt_gbp_updated
              + ' (Today low:' + (metals_data.ounce_price_usd_today_low * metals_data.usd_to_gbp ).toFixed(2)
              + ' high:' + (metals_data.ounce_price_usd_today_high * metals_data.usd_to_gbp ).toFixed(2)
              + ')'
             );

}
