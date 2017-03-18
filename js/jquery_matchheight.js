/**
 * @file
 * jQuery matchHeight module javascript settings.
 */

(function($) {

  $('.matchheight').each(function() {
      $(this).children('.item').matchHeight({
          byRow: false
      });
  });

})(jQuery);
