jQuery(document).ready(function($) {


// загрузка при скроллинге

$('.main-col img, .mid-col img, .sidebar img').addClass('no-display');
 $('.main-col img, .mid-col img, .sidebar img').one('inview', function() {
 $(this).addClass('appear');
   });
   
 
//меню

  $("ul.nav-menu,ul.top-nav-menu")
.superfish({
pathClass : 'current',
animation : {opacity:'show',height:"show"},
delay : 400
});
		

//мобильное меню

$('.top-nav-menu').mobileMenu({
    defaultText: 'Открыть меню...',
    className: 'select-menu',
    subMenuDash: '&ndash;'
});

$('.nav-menu').mobileMenu({
    defaultText: 'Открыть меню...',
    className: 'select-menu-main',
    subMenuDash: '&ndash;'
});
          

//всполывающие подсказки
   $('.tagcloud a, #get_recent_comments_wrap li a, .topsocial a, #wp-calendar a, a#backtop,.nice-cats a,.news-date a,.data-col a, .cat-index a').tipsy({fade: true, gravity: 's'});
$('.tooldown, .tooltip-s').tipsy({fade: true, gravity: 'n'});

		 	$(window).scroll(function () {
			if ( $(this).scrollTop() > 100) {
			$('a[href=#top]').fadeIn();
			} else {
				$('a[href=#top]').fadeOut();
			}
		});
		$('a[href=#top]').click(function(){
		$('html, body').animate({scrollTop:0}, 'slow');
			return false;
		});	
		
		$("#showticker").removeClass('no-display');
  
	});