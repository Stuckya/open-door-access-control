package com.github.stuckya.dooraccessserver.domain.persistence;

import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.experimental.SuperBuilder;

import javax.persistence.Entity;
import javax.persistence.PrimaryKeyJoinColumn;

@SuperBuilder
@Getter
@NoArgsConstructor
@Entity
@PrimaryKeyJoinColumn(name="id")
public class WiegandCredential extends Credential<WiegandCredential> {

    private int code;
}
