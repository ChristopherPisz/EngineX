#ifndef FONT2D_H
#define FONT2D_H

#include "Graphics\2D\TextureCoordRect.h"

#include <d3d9.h>
#include <d3dx9.h>

#include <string>
#include <vector>

/*
Image guidelines:
* Fonts may consist of a minimum of 95 and up to 255 glyphs
* The first 95 should correspond to the printable characters of the ASCII set, beginning with ASCII code 32 (space) and ending with 126 (tilde) 
* Rectangles containing glyphs should be uniform in height and width
* Glyphs must be uniform in height, but are not required to be uniform in width

Rectanglular areas of the image that compose the glyph will be calulated in the following manner:
* Split the image into rectangles of equal width and height starting at the top left, going right for 16 rectangles and then starting a new row,
  until the image has been split into enough rows of 16 rectangles to contain the number of glyphs in the font
* The width of each rectangle should be equal to the image width / 16
* The height of each rectangle should equal the image height / the number of rows
* Number the rectangles starting with 1 to the number of glyphs
* 0 will be reserved for a null character and will not be displayed or represented in the image
* Each rectangle should contain the image of a single glyph

Glyph Dimensions
* The actual texture x coordinates of a glyph will be calculated using the character width / 2 from either side of the rectangle containing the glyph
* The actual texture y coordinates of a glyph will be calculated using the rectangle height

Encoding:
* Using the guidelines above, the ASCII codes in a text string will map to rectangle numbers as follows

Rect #	Ascii	Character     | Rect #	Ascii	Character     | Rect #	Ascii	Character     | Rect #	Ascii	Character
----------------------------------------------------------------------------------------------------------------------------
None	   0	Not Displayed |   64	  95	_             |  128	None	Custom        |  192	None	Custom	    
   1	  32	Space         |	65	  96	'             |  129	None	Custom        |  193	None	Custom
   2	  33	!             |   66	  97	a             |  130	None	Custom        |  194	None	Custom
   3	  34	"             |   67	  98	b             |  131	None	Custom        |  195	None	Custom
   4	  35	#             |   68	  99	c             |  132	None	Custom        |  196	None	Custom
   5	  36	$             |   69	 100	d             |  133	None	Custom        |  197	None	Custom
   6	  37	%             |   70	 101	e             |  134	None	Custom        |  198	None	Custom
   7	  38	&             |   71	 102	f             |  135	None	Custom        |  199	None	Custom
   8	  39	'             |   72	 103	g             |  136	None	Custom        |  200	None	Custom
   9	  40	(             |   73	 104	h             |  137	None	Custom        |  201	None	Custom
  10	  41	)             |   74	 105	i             |  138	None	Custom        |  202	None	Custom
  11	  42	*             |   75	 106	j             |  139	None	Custom        |  203	None	Custom
  12	  43	+             |   76	 107	k             |  140	None	Custom        |  204	None	Custom
  13	  44	'             |   77	 108	l             |  141	None	Custom        |  205	None	Custom
  14	  45	-             |   78	 109	m             |  142	None	Custom        |  206	None	Custom
  15	  46	.             |   79	 110	n             |  143	None	Custom        |  207	None	Custom
  16	  47	/             |   80	 111	o             |  144	None	Custom        |  208	None	Custom
  17	  48	0             |   81	 112	p             |  145	None	Custom        |  209	None	Custom
  18	  49	1             |   82	 113	q             |  146	None	Custom        |  210	None	Custom
  19	  50	2             |   83	 114	r             |  147	None	Custom        |  211	None	Custom
  20	  51	3             |   84	 115	s             |  148	None	Custom        |  212	None	Custom
  21	  52	4             |   85	 116	t             |  149	None	Custom        |  213	None	Custom
  22	  53	5             |   86	 117	u             |  150	None	Custom        |  214	None	Custom
  23	  54	6             |   87	 118	v             |  151	None	Custom        |  215	None	Custom
  24	  55	7             |   88	 119	w             |  152	None	Custom        |  216	None	Custom
  25	  56	8             |   89	 120	x             |  153	None	Custom        |  217	None	Custom
  26	  57	9             |   90	 121	y             |  154	None	Custom        |  218	None	Custom
  27	  58	:             |   91	 122	z             |  155	None	Custom        |  219	None	Custom
  28	  59	;             |   92	 123	{             |  156	None	Custom        |  220	None	Custom
  29	  60	<             |   93	 124	|             |  157	None	Custom        |  221	None	Custom
  30	  61	=             |   94	 125	}             |  158	None	Custom        |  222	None	Custom
  31	  62	>             |   95	 126	~             |  159	None	Custom        |  223	None	Custom
  32	  63	?             |   96	None	Custom        |  160	None	Custom        |  224	None	Custom
  33	  64	@             |   97	None	Custom        |  161	None	Custom        |  225	None	Custom
  34	  65	A             |   98	None	Custom        |  162	None	Custom        |  226	None	Custom
  35	  66	B             |   99	None	Custom        |  163	None	Custom        |  227	None	Custom
  36	  67	C             |  100	None	Custom        |  164	None	Custom        |  228	None	Custom
  37	  68	D             |  101	None	Custom        |  165	None	Custom        |  229	None	Custom
  38	  69	E             |  102	None	Custom        |  166	None	Custom        |  230	None	Custom
  39	  70	F             |  103	None	Custom        |  167	None	Custom        |  231	None	Custom
  40	  71	G             |  104	None	Custom        |  168	None	Custom        |  232	None	Custom
  41	  72	H             |  105	None	Custom        |  169	None	Custom        |  233	None	Custom
  42	  73	I             |  106	None	Custom        |  170	None	Custom        |  234	None	Custom
  43	  74	J             |  107	None	Custom        |  171	None	Custom        |  235	None	Custom
  44	  75	K             |  108	None	Custom        |  172	None	Custom        |  236	None	Custom
  45	  76	L             |  109	None	Custom        |  173	None	Custom        |  237	None	Custom
  46	  77	M             |  110	None	Custom        |  174	None	Custom        |  238	None	Custom
  47	  78	N             |  111	None	Custom        |  175	None	Custom        |  239	None	Custom
  48	  79	O             |  112	None	Custom        |  176	None	Custom        |  240	None	Custom
  49	  80	P             |  113	None	Custom        |  177	None	Custom        |  241	None	Custom
  50	  81	Q             |  114	None	Custom        |  178	None	Custom        |  242	None	Custom
  51	  82	R             |  115	None	Custom        |  179	None	Custom        |  243	None	Custom
  52	  83	S             |  116	None	Custom        |  180	None	Custom        |  244	None	Custom
  53	  84	T             |  117	None	Custom        |  181	None	Custom        |  245	None	Custom
  54	  85	U             |  118	None	Custom        |  182	None	Custom        |  246	None	Custom
  55	  86	V             |  119	None	Custom        |  183	None	Custom        |  247	None	Custom
  56	  87	W             |  120	None	Custom        |  184	None	Custom        |  248	None	Custom
  57	  88	X             |  121	None	Custom        |  185	None	Custom        |  249	None	Custom
  58	  89	Y             |  122	None	Custom        |  186	None	Custom        |  250	None	Custom
  59	  90	Z             |  123	None	Custom        |  187	None	Custom        |  251	None	Custom
  60	  91	[             |  124	None	Custom        |  188	None	Custom        |  252	None	Custom
  61	  92	\             |  125	None	Custom        |  189	None	Custom        |  253	None	Custom
  62	  93	]             |  126	None	Custom        |  190	None	Custom        |  254	None	Custom
  63	  94	^             |  127	None	Custom        |  191	None	Custom        |  255	None	Custom

*/

//-------------------------------------------------------------------------------
class Font2D
{
public:
	typedef std::vector<int>				GlyphWidths;				// Width in pixels of each character in the font

	Font2D(LPDIRECT3DDEVICE9 device = NULL);
	Font2D(const Font2D & rhs);
	virtual ~Font2D();

	virtual Font2D & operator = (const Font2D & rhs);

	// Load the font from an image file and initialize the font
	virtual void CreateFont(std::string name, std::string path, unsigned numGlyphs, GlyphWidths & widths, unsigned int fontHeight);

	// Retrieve texture coordinates for a given glyph
//	virtual const TextureCoordRect & GetRectForGlyph(int glyphIndex) const;

protected:
	std::string								m_name;						// The name of the font
	unsigned int							m_numGlyphs;				// How many glyphs are in the font
	unsigned int							m_height;					// Height of the font in pixels
	GlyphWidths								m_glyphWidths;				// Width in pixels of each character in the font				

	typedef std::vector<TextureCoordRect>	GlyphRects;					// Texture coordinates for rectangles that encompass each glyph  
	GlyphRects								m_glyphRects;

	LPDIRECT3DDEVICE9						m_device;					// The Direct3D device
	LPDIRECT3DTEXTURE9               m_texture;					// Texture containing the entire font
};

#endif
