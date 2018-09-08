var blockStart = '@d'; // drop
var itemStart = '@s'; // scroll

function doGet() {

  var result = [];
  
  getDate(result);
  getTime(result);

  getCalendarEvents(result,  'family', 4, 'date');
  showTime(result);
  
  getRss(result, 'UK News', 4, 'http://feeds.bbci.co.uk/news/rss.xml?edition=uk');
  showTime(result);
  
  getRss(result, 'Kirkcaldy Weather', 3, 'https://weather-broker-cdn.api.bbci.co.uk/en/forecast/rss/3day/2645298');
  showTime(result);

  getFinancial(result);
  showTime(result);
  
  //Expected deliveries (Asda, Amazon)
  //showTime(result);
  
  //Countdown to ... Something
  getCalendarEvents(result,  'countdown', 4, 'days');
  showTime(result);
  
  //Last 5 tweets from ... X account
  getTweets(result, 5);
  showTime(result);
  
  //Last DM to ... Y Twitter account
  //showTime(result);
  
  return ContentService
  .createTextOutput(result.join("\n"))
  .setMimeType(ContentService.MimeType.TEXT);
  
}



