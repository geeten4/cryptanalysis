trojice (i, j, hodnota)
serazeny (i, j) < (k, l) (i < k) || (iff i=k && j < l)
halda uvnitr haldy?
radky vzdy nenulove, jen malo zaznamu v radku nenulovych

gauss elminace:
    1) najdu radek s nejnizsim pivotem, vymenim s akutalnim
    2) vyskaluju inverzem pivotu cely radek
    3) od kazdeho radku pod nim odectu pivotni radek
    4) zpetnou substituci dostavam reseni

operace:
    - prohozeni radku
    - vynasobeni radku cislem
    - pricitani k-nasobku k jinemu radku

pro vektor:
    - halda v poli - prvky (j, x) , j cislo sloupce, x cislo
    - operace:
        - inicializace
        - free
        - print
        - insert_value
        - get_value
        - increment
        - next_nonzero
        - express_in_fbasis
        - multiply_with_fbasis
        - scale
        - add_vector_multiple


pro matici:
    - halda v poli - prvky (i, vp), i cislo radku, vp pointer na vektor

void Vector_init(Vector *v);
void Vectore_free(Vector *v);
void Vector_print(Vector *v);
int Vector_insert_value(Vector *v, int index, fb_t value);
fb_t Vector_get_value(Vector *v, int index);
void Vector_increment(Vector *v, int index);
int Vector_next_nonzero_index(Vector *v, int index);

// factor basis
fb_t Vector_express_in_fbasis(Vector *v, BasisList *fb, fb_t n);
fb_t Vector_multiply_with_fbasis(Vector *v, BasisList *fb);

// linear manipulation
void Vector_scale(Vector *v, fb_t c, gf_t p);  // multiply v by c modulo p
void Vector_add_vector_multiple(Vector *v, fb_t c, Vector *w, gf_t p);  // to v add c*w modulo p