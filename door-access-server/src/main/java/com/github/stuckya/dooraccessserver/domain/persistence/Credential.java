package com.github.stuckya.dooraccessserver.domain.persistence;

import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.experimental.SuperBuilder;
import org.hibernate.annotations.Type;

import javax.persistence.*;
import java.time.Instant;

@SuperBuilder
@Getter
@NoArgsConstructor
@Entity
@Inheritance(strategy= InheritanceType.JOINED)
public abstract class BaseCredential <T extends BaseCredential<T>> extends BaseEntity<T> {

    private String name;
    private Instant lastUsed;

    @Enumerated(EnumType.STRING)
    @Type(type = "pgsql_enum")
    private CredentialStatus status;
}
