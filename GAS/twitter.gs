

function getTwitterService() {
  // Create a new service with the given name. The name will be used when
  // persisting the authorized token, so ensure it is unique within the
  // scope of the property store.
  return OAuth1.createService('twitter')
      // Set the endpoint URLs.
      .setAccessTokenUrl('https://api.twitter.com/oauth/access_token')
      .setRequestTokenUrl('https://api.twitter.com/oauth/request_token')
      .setAuthorizationUrl('https://api.twitter.com/oauth/authorize')
      // Set the consumer key and secret.
      .setConsumerKey(TWITTER_CONSUMER_KEY)
      .setConsumerSecret(TWITTER_CONSUMER_SECRET)
      // Set the name of the callback function in the script referenced
      // above that should be invoked to complete the OAuth flow.
      .setCallbackFunction('authCallback')
      // Set the property store where authorized tokens should be persisted.
      .setPropertyStore(PropertiesService.getUserProperties());
}

function showSidebar() {
  var twitterService = getTwitterService();
  Logger.log( twitterService.authorize() );
}

function authCallback(request) {
  var twitterService = getTwitterService();
  var isAuthorized = twitterService.handleCallback(request);
  if (isAuthorized) {
    return HtmlService.createHtmlOutput('Success! You can close this tab.');
  } else {
    return HtmlService.createHtmlOutput('Denied. You can close this tab');
  }
}

function getTweets( result, maxNumber ) {
  Logger.log('Entered getTweets');
  
  result.push( blockStart, 'Twitter Home');
  
  var twitterService = getTwitterService();
  var json_response = twitterService.fetch('https://api.twitter.com/1.1/statuses/home_timeline.json?exclude_replies=true&count=' + maxNumber);
  var twitter_data = JSON.parse(json_response);
  
  for (var count in twitter_data) {
    result.push( itemStart, 
                twitter_data[count].user.name + ' : ' 
                + twitter_data[count].text
                   .replace(/\n/g, " | ") 
                );
  }
  
}
