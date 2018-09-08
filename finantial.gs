function getFinancial(result) {
  Logger.log('Entered getFinancial');
  result.push( blockStart, 'Financial Prices');
  
  //fromWebPage(result, 'Gold Price', 'http://www.bullionbypost.co.uk/gold-price/live-gold-price/', 'gold-price-per-ounce');
  getStocks(result, 'Stock Price', [ 'MA', 'GSK' ] );
  getCurrency(result, 'Currency Price', [ 'EUR', 'USD', 'JPY' ] );
  getMetalsPrice( result, 'Gold Price' );
  
}
