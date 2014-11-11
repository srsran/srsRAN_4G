
#ifdef nocompile
// n_drms_2 table 5.5.2.1.1-1 from 36.211
uint32_t n_drms_2[8] = { 0, 6, 3, 4, 2, 8, 10, 9 };

// n_drms_1 table 5.5.2.1.1-2 from 36.211
uint32_t n_drms_1[8] = { 0, 2, 3, 4, 6, 8, 9, 10 };


/* Generation of the reference signal sequence according to Section 5.5.1 of 36.211 */ 
int rs_sequence(ref_t * refs, uint32_t len, float alpha, uint32_t ns, uint32_t cell_id,
                refsignal_ul_cfg_t * cfg)
{
  uint32_t i;

  // Calculate u and v 
  uint32_t u, v;
  uint32_t f_ss = (((cell_id % 30) + cfg->delta_ss) % 30);
  printf("f_ss: %d\n", f_ss);
  if (cfg->group_hopping_en) {
    sequence_t seq; 
    bzero(&seq, sizeof(sequence_t));
    sequence_LTE_pr(&seq, 160, cell_id / 30);
    uint32_t f_gh = 0;
    for (i = 0; i < 8; i++) {
      f_gh += (((uint32_t) seq.c[8 * ns + i]) << i);
    }
    printf("f_gh: %u\n", f_gh);
    sequence_free(&seq);
    u = ((f_gh%30) + f_ss) % 30;
  } else {
    u = f_ss % 30;
  }

  if (len < 6 * RE_X_RB) {
    v = 0;
  } else {
    if (!cfg->group_hopping_en && cfg->sequence_hopping_en) {
      sequence_t seq; 
      bzero(&seq, sizeof(sequence_t));
      sequence_LTE_pr(&seq, 20, ((cell_id / 30) << 5) + f_ss);    
      v = seq.c[ns];
      sequence_free(&seq);
    } else {
      v = 0;
    }
  }
  printf("u: %d, v: %d\n", u, v);
  if (len >= 3 * RE_X_RB) {
    uint32_t n_sz;
    uint32_t q;
    float q_hat;
    /* get largest prime n_zc<len */
    for (i = NOF_PRIME_NUMBERS - 1; i > 0; i--) {
      if (prime_numbers[i] < len) {
        n_sz = prime_numbers[i];
        break;
      }
    }
    printf("n_sz: %d\n", n_sz);
    q_hat = (float) n_sz *(u + 1) / 31;
    if ((((uint32_t) (2 * q_hat)) % 2) == 0) {
      q = (uint32_t) (q_hat + 0.5) + v;
    } else {
      q = (uint32_t) (q_hat + 0.5) - v;
    }
    cf_t *x_q = malloc(sizeof(cf_t) * n_sz);
    if (!x_q) {
      perror("malloc");
      return LIBLTE_ERROR;
    }
    for (i = 0; i < n_sz; i++) {
      x_q[i] =
        cexpf(-I * M_PI * (float) q * (float) i * ((float) i + 1) / n_sz);
    }

    for (i = 0; i < len; i++) {
      refs[i].simbol = cfg->beta * cexpf(I * alpha * i) * x_q[i % n_sz];
    }
    free(x_q);
  } else {
    if (len == RE_X_RB) {
      for (i = 0; i < len; i++) {
        refs[i].simbol = cfg->beta * cexpf(I * (phi_M_sc_12[u][i] * M_PI / 4 + alpha * i));
      }
    } else {
      for (i = 0; i < len; i++) {
        refs[i].simbol = cfg->beta * cexpf(I * (phi_M_sc_24[u][i] * M_PI / 4 + alpha * i));
      }
    }
  }

  return LIBLTE_SUCCESS;
}

/** Initializes refsignal_t object according to 3GPP 36.211 5.5.2
 *
 */
int refsignal_init_LTEUL_drms_pusch(refsignal_t * q, uint32_t nof_prb, uint32_t prb_start, 
                             uint32_t nslot, lte_cell_t cell, refsignal_ul_cfg_t * cfg)
{

  uint32_t i;
  int ret = LIBLTE_ERROR_INVALID_INPUTS;
  uint32_t n_prs;
  uint32_t M_sc; 
  float alpha;

  if (q != NULL && nslot < NSLOTS_X_FRAME && lte_cell_isvalid(&cell)) {

    bzero(q, sizeof(refsignal_t));

    M_sc = nof_prb * RE_X_RB;

    q->nof_refs = M_sc;
    q->nsymbols = 1;
    q->voffset = cell.id % 6;
    q->nof_prb = cell.nof_prb;

    q->symbols_ref = malloc(sizeof(uint32_t) * 1);
    if (!q->symbols_ref) {
      perror("malloc");
      goto free_and_exit;
    }

    if (CP_ISNORM(cell.cp)) {
      q->symbols_ref[0] = 3;
    } else {
      q->symbols_ref[0] = 2;
    }

    q->refs = vec_malloc(q->nof_refs * sizeof(ref_t));
    if (!q->refs) {
      goto free_and_exit;
    }
    q->ch_est = vec_malloc(q->nof_refs * sizeof(cf_t));
    if (!q->ch_est) {
      goto free_and_exit;
    }

    /* Calculate n_prs */
    uint32_t c_init; 
    sequence_t seq; 
    bzero(&seq, sizeof(sequence_t));
    c_init = ((cell.id / 30) << 5) + (((cell.id % 30) + cfg->delta_ss) % 30);
    ret = sequence_LTE_pr(&seq, 8 * CP_NSYMB(cell.cp) * 20, c_init);
    if (ret != LIBLTE_SUCCESS) {
      goto free_and_exit;
    }
    n_prs = 0;
    for (i = 0; i < 8; i++) {
      n_prs += (seq.c[8 * CP_NSYMB(cell.cp) * nslot + i] << i);
    }
    sequence_free(&seq);
    
    // Calculate cyclic shift alpha
    uint32_t n_cs =
      (n_drms_1[cfg->cyclic_shift] +
       n_drms_2[cfg->cyclic_shift_for_drms] + n_prs) % 12;
    alpha = 2 * M_PI * (n_cs) / 12;

    printf("alpha: %g\n", alpha);
    
    if (rs_sequence(q->refs, M_sc, alpha, nslot, cell.id, cfg)) {
      fprintf(stderr, "Error generating RS sequence\n");
      goto free_and_exit; 
    }
    /* mapping to resource elements */
    for (i=0;i<M_sc;i++) {
      q->refs[i].freq_idx = prb_start*RE_X_RB + i;
      q->refs[i].time_idx = q->symbols_ref[0];      
    }
  
    ret = LIBLTE_SUCCESS;
  }
free_and_exit:
  if (ret == LIBLTE_ERROR) {
    refsignal_free(q);
  }
  return ret;
}

#endif