/**
 * @file
 * Javascript functions
 */

Drupal.behaviors.upei_roblib_orcid = {
  attach: function (context, settings) {
    /*
     *  If the Drupal logout button/url has an id of orcid-and-drupal-logout we
     *  will try to log you out of ORCID as well as Drupal.  Only required
     *  in a situation where computer access is shared with several users and
     *  browser cache is not cleared between users.
     */
    jQuery("#orcid-and-drupal-logout").click(function () {
      var orcid_logout_url = Drupal.settings.upei_roblib_orcid.orcid_logout_url;
      jQuery.getJSON(orcid_logout_url, function(data) {
        jQuery.each(data, function(key, val) {
          console.log(key + " -- " + val);
        })
          console.log("Attempted to log out of ORCID. Still Logged In: " + data.loggedIn);
      });
    })
  }
}
